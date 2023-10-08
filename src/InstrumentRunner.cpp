#include <InstrumentRunner.h>

//SoftwareSerial debug(8,10);

uint8_t InstrumentRunner::id = 0;
uint8_t InstrumentRunner::type = 0;
uint16_t InstrumentRunner::bodRate = 0;
uint16_t InstrumentRunner::timeout = 0;
uint16_t InstrumentRunner::interval = 0;
uint16_t InstrumentRunner::T35 = 0;
uint16_t InstrumentRunner::T240 = 0;

Interface * interface;
Modbus modbus;
Tenzo tenzoSensor;
Magnet magnetSensor;
Encoder encoder;
//              eeprom loads table
// slave_id speed_uart config_uart interval_uart  wire_encoder
//    6         7           8           9             10     
void InstrumentRunner::init(){  
    cli();
    initTimer();
    sei();

    //debug.begin(9600);
    //while(!debug);
    
    interface = nullptr;

    uint8_t slaveId = EEPROM.readByte(6);
    uint8_t speed_uart = EEPROM.readByte(7);
    uint8_t config_uart = EEPROM.readByte(8);
    uint8_t interval_uart = EEPROM.readByte(9);
    uint8_t wire_encoder = EEPROM.readByte(10);
    
    if(slaveId > 32 || slaveId == 0){
        id = 3;         //default
    }else{
        id = slaveId;
    }
    //debug.println((int)slaveId);
    //debug.println((int)speed_uart);
    //debug.println((int)config_uart);
    //debug.println((int)interval_uart);
    //debug.println((int)wire_encoder);

    if(speed_uart > 5 || speed_uart == 0){
        bodRate = 9600; //default 
    }else{
        bodRate = 600 << speed_uart;
    }
    if(config_uart > 6 || config_uart == 0){
        type = SERIAL_8O1;   
    }else{
        switch (config_uart)
        {
            case 1:
                type = SERIAL_8N1;
                break;
            case 2:
                type = SERIAL_8N2;
                break;
            case 3:
                type = SERIAL_8E1;
                break;
            case 4:
                type = SERIAL_8E2;
                break;
            case 5: 
                type = SERIAL_8O1;
                break;
            case 6:
                type = SERIAL_8O2;
                break;
        }
    }
    
    if(interval_uart > 16 || interval_uart == 0){
        interval = 120; //10*ms default
    }else{
        interval = interval_uart*2*10; //10*ms value
    }

    if(wire_encoder > 4 || wire_encoder == 0){
        interface = new TWIwiring(); //default setup
    }else{
        switch (wire_encoder)
        {   
            case 1:
                interface = new TWIwiring();
                break;
            case 2:
                interface = new SPIwiring();
                break;
            case 3:
                //interface = new ABwiring();  //not finished implementation
                break;
            case 4:
                //interface = new ABZwiring(); //not finished implementation
                break;
        }
    }
    
    uint8_t pwm_tension = EEPROM.readByte(20);
    uint16_t correction_tension = EEPROM.readInt(12);
    uint16_t shift_adc = EEPROM.readInt(25);

    uint8_t pwm_magnet = EEPROM.readByte(14);
    uint32_t depth_enc = EEPROM.readLong(21);
    uint8_t inv_enc = EEPROM.readByte(19);


    //init sensors
    encoder.setInterface(interface);
    encoder.init(depth_enc, inv_enc);
    tenzoSensor.init(pwm_tension, correction_tension, shift_adc);
    magnetSensor.init(pwm_magnet);
    
    modbus.init(bodRate, type, 1000);
    modbus.setId(id);
    updateTimestamp();

    //debug.println(bodRate);
    
    return;
}

void InstrumentRunner::updateTimestamp(){
    uint8_t bitsPerChar;
    if (type == SERIAL_8E2 || type == SERIAL_8O2) bitsPerChar = 12;
    else if (type == SERIAL_8N2 || type == SERIAL_8E1 || type == SERIAL_8O1) bitsPerChar = 11;
    else bitsPerChar = 10;
    uint16_t kBit_velocity = bodRate/bitsPerChar; //speed in byte/s
    // t = size/kBsec
    T35 = 3.5*10000/kBit_velocity; //10*ms
    T240 = 24*10000/kBit_velocity; //10*ms
}

void InstrumentRunner::run(){
    static volatile uint64_t timer = 0;
    static uint8_t state = 0;
    static uint64_t t = 0;
    static uint64_t t2 = 0;
    encoder.updateCount();  //read encoder 
    encoder.EEPROMSignalCheck();  //if power is shutdown start write in EEPROM encoder value
    if(_micros() - t2 >= 100){  //old 100 t/e 10 ms 100hz
        readSensors(); 
        t2 = _micros();
    }

    if(_micros() - t > 1){    
        
        //modbus rtu automat
        if(state == 0 && _micros() - timer >= T240 && ! modbus.bufferEmpty()){ //+ timing t240 after sending prev answer ot master
            modbus.poll(); //start reading
            timer = _micros(); //remember time after success reading
            state++; 
        }
        else if(state == 1){  
            uint8_t status = modbus.validate();   
            // for (uint8_t i = 0; i < 16; i++)
            // {
            //     debug.printf("%02X", status[i]);  //set speed 03 06 75 02 00 04 32 27
            //     debug.print(" ");                 //set bitcnt 03 06 75 0A 00 00 B2 26
            // }
            // debug.println();
            // modbus.clearBufferInput();
            // state = 0;

            if(status == 0){ //if input command is validated
                
                if(modbus.defineAndExecute() == 0){ //if success executed command
                    state++;
                }else{
                    state = 0;
                }
            }
            else if(status == 1){   //not for this device 
                state = 0;   
                //debug.println("id");
            }
            else if(status == 2){  // crc error
                state = 0;
                //debug.println("crc");
            }
            else if(status == 3){
                state = 0;
                //debug.println("trash");
            }
        }
        else if(state == 2 && _micros() - timer >= (T35 + interval)){ //timing t35 + interval
            modbus.query(); // send answer to master device
            //debug.println("send");
            timer = _micros();    //remember time after success sending
            state = 0;
        }           
        t = _micros();
    }
}
  
void InstrumentRunner::readSensors(){
    tenzoSensor.updateForce();
    magnetSensor.updateMagnet();
}

static uint16_t hi_part , lo_part; 

uint16_t InstrumentRunner::getValue(uint8_t key){
    if(key == SET_ID){
        return id;
    }
    if(key == SET_SPEED){
        return EEPROM.readByte(7);
    }
    if(key == SET_EVEN){
        
    }
    if(key == SET_INTERVAL){
        return interval/20;
    }
    if(key == SET_COUNT_STOP_BIT){
        
    }
    if(key == SET_ZERO_TENSION){
        //invalid
    }
    if(key == CALIB_TENSION){
        return tenzoSensor.getCorrection();
    }
    if(key == SET_ENCODER_WIRE){
        //debug.println("wire");
        return EEPROM.readByte(10);
    }
    if(key == SET_INVERT_ENCODER){
        //debug.println("inv");
        if(encoder.getInvert()){
            return 1;
        }
        return 0;
    }
    if(key == CALIB_HALL){
        return magnetSensor.getCalibrate();
    }
    if(key == GET_TENSION){
        return tenzoSensor.getForce();
    }
    if(key == GET_ENCODER_HI){
        uint32_t tmp = encoder.getCount();
        hi_part =  (tmp >> 16) & 0xFFFF;
        lo_part = tmp & 0xFFFF;
        return hi_part;
    }
    if(key == GET_ENCODER_LO){
        return lo_part;
    }
    if(key == GET_HALL){
        return magnetSensor.getMagnet();
    }
    return 0;
}

uint32_t hi_val;
uint8_t tempSpeed;
//              eeprom loads table
// slave_id speed_uart config_uart interval_uart  wire_encoder
//    6         7           8           9             10     
void InstrumentRunner::setValue(uint8_t key, uint8_t highByte, uint8_t lowByte){
    static uint8_t tempType;
    if(key == SET_ID){
        modbus.setId(lowByte);
        EEPROM.writeByte(6, lowByte);
    }
    if(key == SET_SPEED){
        //update bod rate 
        //debug.println('s');
        switch (lowByte)
        {
            case 1:
                bodRate = 1200;
                break;
            case 2:
                bodRate = 2400;
                break;
            case 3:
                bodRate = 4800;
                break;
            case 4:
                bodRate = 9600;
                break;
            case 5:
                bodRate = 19200;
                break;
        }
        tempSpeed = lowByte;
    }
    if(key == SET_EVEN){
        //debug.println('e');
        tempType = lowByte;
    }
    if(key == SET_INTERVAL){
        //debug.println('i');
        interval = 2*lowByte*10;
        EEPROM.writeByte(9, lowByte);
    }
    if(key == SET_COUNT_STOP_BIT){
        //this command must recived latest for correct update serial
        tempType += lowByte;
        //debug.println('z');
        switch (tempType)
        {
            case 1:
                type = SERIAL_8N1;
                break;
            case 2:
                type = SERIAL_8N2;
                break;
            case 3:
                type = SERIAL_8E1;
                break;
            case 4:
                type = SERIAL_8E2;
                break;
            case 5: 
                type = SERIAL_8O1;
                break;
            case 6:
                type = SERIAL_8O2;
                break;
        }
        modbus.update(bodRate, type, timeout);
        updateTimestamp();
        EEPROM.writeByte(8,tempType);
        EEPROM.writeByte(7,tempSpeed);
    }
    if(key == SET_ZERO_TENSION){
        tenzoSensor.setZero();
    }
    if(key == CALIB_TENSION){
        uint16_t weight = (highByte << 8) | lowByte;
        tenzoSensor.calibrate(weight);
    }
    if(key == SET_ENCODER_WIRE){
        delete interface;
        interface = nullptr;
        switch (lowByte)
        {
        case 1:
            interface = new TWIwiring();
            EEPROM.writeByte(10,lowByte);
            break;
        case 2:
            interface = new SPIwiring();
            EEPROM.writeByte(10,lowByte);
            break;
        case 3:
            //not finished implementation
            break;
        case 4:
            //not finished implementation
            break;
        }
        interface->init();
        encoder.setInterface(interface);
    }
    if(key == SET_INVERT_ENCODER){
        bool invert = false;
        if(lowByte == 1){
            invert = true;
        }
        encoder.setInvert(invert);
        EEPROM.writeByte(19, lowByte); // 0 - off; 1 - on
    }
    if(key == CALIB_HALL){
        magnetSensor.setPWM(lowByte);
    }
    if(key == GET_TENSION){
        //invalid
    }
    if(key == GET_ENCODER_HI){
        //set enc val
        hi_val = (highByte << 8) | lowByte;
    }
    if(key == GET_ENCODER_LO){
        //set enc val
        uint16_t lo_val = (highByte << 8) | lowByte;
        uint32_t encode = (hi_val << 16) | lo_val;
        encoder.setCount(encode);
    }
    if(key == GET_HALL){
        //invalid
    }
    if(key == ACCEPT_HALL){
        magnetSensor.EEPROMwrite();
    }
}

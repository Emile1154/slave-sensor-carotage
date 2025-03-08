#include <sensors/Sensors.h>
Encoder::Encoder()
{
    
}

Encoder::~Encoder()
{
    delete interface;
    interface = nullptr;
}

void Encoder::setInterface(Interface* interface){
    this->interface = interface;
    interface->init();
}
uint32_t angle = 0;
uint32_t prevAngle = 0;  
void Encoder::init(uint32_t count, uint8_t inv){
    invert = false;
    if(inv == 1){
        invert = true;
    }
    this->count = count;    
    
    _delay_ms(500);
    // if(count >= 10000000){
    //     count = count - prevAngle;
    // }else{
    //     count = count + prevAngle;
    // }
    prevAngle = interface->calculateAngle();  //fix on start
    angle = prevAngle;

    if(interface->interface_code == 4){
        interface->val = count;
    }

    //interrupt init save enc count
    DDRC &= ~(1 << EEPROM_SIGNAL_PIN); 
    PCICR |= (1 << PCIE1);
    PCMSK1 |= (1 << PCINT8);
}

float delta = 0;
#define STEP 360


void Encoder::updateCount(){
    
    if(interface->interface_code == 1){
        angle = interface->calculateAngle(); // 0 : 360 000
        delta = (int32_t)(angle - prevAngle);
        prevAngle = angle;
        //correction
        if (delta >= 180000) {
            delta -= 360000;
        } 
        else if (delta <= -180000) {
            delta += 360000;
        }
        if (invert) {
            delta *= -1;
        }
        count += delta; //а здесь count uint32_t и мы теряем дробные значения
    }
    else if(interface->interface_code == 3){ 
        angle = interface->calculateAngle(); // 0 : 360 000
        if(angle != prevAngle){ 
            if( (angle == 3 && prevAngle == 1) || (angle == 0 && prevAngle == 2) ){
                delta = STEP;
            }
            else if( (angle == 2 && prevAngle == 0) || (angle == 1 && prevAngle == 3) ){
                delta = -STEP;
            }
            if (invert) {
                delta *= -1;
            }
            count += delta; 
            prevAngle = angle;
        }
    }        
    // else if(interface->interface_code == 4){
    //     count = angle;
    // }
}

uint32_t Encoder::getCount(){
    return count;
}

Interface * Encoder::getInterface(){
    return interface;
}

void Encoder::setCount(uint32_t count){
    this->count = count; //360.0 => 360000
}

void Encoder::setInvert(bool invert){
    this->invert = invert;
}

bool Encoder::getInvert(){
    return invert;
}


void Encoder::EEPROMSignalCheck(){
    if(PINC & (1 << EEPROM_SIGNAL_PIN)){
        writeFlag = true;
        
    }
    if(writeFlag){
        EEPROM.writeLong(15,count);
        while (! EEPROM.isReady());
        writeFlag = false;
    }
}

// bool Encoder::getWriteFlag(){
//     return writeFlag;
// }
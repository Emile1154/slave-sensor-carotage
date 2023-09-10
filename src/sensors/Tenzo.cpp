#include <sensors/Sensors.h>

Tenzo::Tenzo(/* args */)
{
}

Tenzo::~Tenzo()
{
}

void Tenzo::init(){
    adc_init();
    DDRD |= (1 << TENSION_PWM_OUTPUT); //set output mode
    // 62.5 kHz
    TCCR0B = 0b00000001;  // x1
    TCCR0A = 0b00000011;  // fast pwm
    TCCR0A |=  (1 << COM0A1);

    OCR0A = EEPROM.readByte(11);
    correction = EEPROM.readInt(12);
}

void Tenzo::setZero(){
    uint16_t temp = readADC();
    if (temp > 200)
    {
        while (1)
        {
            uint16_t val = readADC();
            if (val <= 190)
            {
                shift_adc = val;
                break;
            }
            pwm_tension--;
            OCR0A = pwm_tension;
        }
    }
    else if (temp < 200)
    {
        while (1)
        {
            uint16_t val = readADC();
            if (val >= 200)
            {
                shift_adc = val; //need save in eeprom too
                break;
            }
            pwm_tension++;
            OCR0A = pwm_tension;
        }
    }
    //write eeprom
    while(1){
        if(EEPROM.writeByte(11, pwm_tension) == true){
            break;
        }
        
    }
    
}

uint8_t Tenzo::getPWM(){
    return pwm_tension;
}

void Tenzo::calibrate(uint16_t weight){
    /*
        weight = 10kg;
        adc = 70;
        10 kg = 70*correction/1023;
        correction = weight[KG]x1023/ADC_VALUE 
    */
    //uint16_t val = readADC();
    //correction = weight*1023/val;
    //eeprom write
    correction = weight; // for test
    while (1)
    {
        if(EEPROM.writeInt(12,correction) == true){
            break;
        }
    }
}

void Tenzo::updateForce(){
    uint16_t adc = readADC();
    uint16_t clean_adc; 
    if(adc >= shift_adc){ // сдвиг значения ацп после установки 0 на 200
        clean_adc = adc - shift_adc; //получаем значение чистого ацп без сдвига
    }else{
        clean_adc = 0;
    }
    uint16_t scaled = (uint16_t) (clean_adc * correction); //телеметрия натяжения 8191кг = 5v
    
    force = scaled/8;
}

uint16_t Tenzo::getForce(){
    return force;
}

uint16_t Tenzo::getCorrection(){
    return correction;
}

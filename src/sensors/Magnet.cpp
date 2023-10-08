#include <sensors/Sensors.h>

Magnet::Magnet()
{
}

Magnet::~Magnet()
{
}

void Magnet::init(uint8_t _pwm){
    //eeprom read

    DDRD &= ~(1 << HOLLA_SENSOR_PIN); //set input mode
    DDRD |= (1 << HOLLA_PWM_OUTPUT); //set output mode
    TCCR0A |=  (1 << COM0B1);
    OCR0B = _pwm;
    pwm_duty = OCR0B;
}
void Magnet::setPWM(uint8_t pwm){
    OCR0B = pwm;
    pwm_duty = pwm;
}
void Magnet::EEPROMwrite(){
    //eeprom write
    while(1){
        if(EEPROM.writeByte(14, pwm_duty)){
            break;
        }
    }
    
}

void Magnet::updateMagnet(){
    magnet = (PIND & (1 << HOLLA_SENSOR_PIN))/8;
}

uint8_t Magnet::getMagnet(){
    return magnet;
}

uint8_t Magnet::getCalibrate(){
    return pwm_duty;
}
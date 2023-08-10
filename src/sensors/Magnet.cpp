#include <sensors/Sensors.h>

Magnet::Magnet()
{
}

Magnet::~Magnet()
{
}

void Magnet::init(){
    //eeprom read

    DDRB |= (0 << HOLLA_SENSOR_PIN); //set input mode
    DDRD |= (1 << HOLLA_PWM_OUTPUT); //set output mode
    TCCR0A |=  (1 << COM0B1);
    OCR0B = EEPROM.readByte(3);
    pwm_duty = OCR0B;
}

void Magnet::calibrate(uint8_t duty){
    OCR0B = duty;
    pwm_duty = duty;
    //eeprom write
    EEPROM.writeByte(3, duty);
}

void Magnet::updateMagnet(){
    magnet = PIND & (1 << HOLLA_SENSOR_PIN);
}

bool Magnet::getMagnet(){
    return magnet;
}

uint8_t Magnet::getCalibrate(){
    return pwm_duty;
}
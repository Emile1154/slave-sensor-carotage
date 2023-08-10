#include <sensors/Sensors.h>

Encoder::Encoder()
{
    
}

Encoder::~Encoder()
{
    delete[] interface;
    interface = nullptr;
}

void Encoder::setInterface(Interface* interface){
    this->interface = interface;
}

void Encoder::init(){
    interface->init();
    invert = -1;
    totalRevolutions = EEPROM.readInt(4);
    DDRC |= (0 << EEPROM_SIGNAL_PIN);
}

void Encoder::updateCount(){
    uint16_t angle = interface->calculateAngle(); // 0 : 3600
    static int16_t prevAngle = 0;  
    if (angle != prevAngle) {
        int16_t derivative = angle - prevAngle;
        
        if (derivative > 1800) {
            derivative -= 3600;
        } else if (derivative < -1800) {
            derivative += 3600;
        }
        float revolutions = derivative / 360.0;
        if(!invert){
            totalRevolutions += revolutions;
        }else{
            totalRevolutions -= revolutions;
        }
        
        prevAngle = angle;
    }
    count = totalRevolutions;
}

uint16_t Encoder::getCount(){
    return count;
}

Interface * Encoder::getInterface(){
    return interface;
}

void Encoder::EEPROMSignalCheck(){
    
    if(PINC & (1 << EEPROM_SIGNAL_PIN)){
        EEPROM.writeInt(4,count);
      
    }
}
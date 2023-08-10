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
    uint16_t angle = interface->calculateAngle(); // 0 : 36000
    static int32_t prevAngle = 0;  
    if (angle != prevAngle) {
        int32_t derivative = angle - prevAngle;
        
        if (derivative > 18000) {
            derivative -= 36000;
        } else if (derivative < -18000) {
            derivative += 36000;
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

void Encoder::updateFrequency(){
    if(count != oldCount){
        frequency = abs(count - oldCount)*100/2;
        oldCount = count;
    }else{
        frequency = 0;
    }
}

uint16_t Encoder::getFrequency(){
    return frequency;
}

uint32_t Encoder::getCount(){
    return count;
}

Interface * Encoder::getInterface(){
    return interface;
}

void Encoder::setCount(uint32_t count){
    totalRevolutions = (float) count/100.0; //360.0 => 36000/100.0
}

void Encoder::setInvert(bool invert){
    this->invert = invert;
}

bool Encoder::getInvert(){
    return invert;
}

void Encoder::EEPROMSignalCheck(){
    
    if(PINC & (1 << EEPROM_SIGNAL_PIN)){
        EEPROM.writeInt(4,count);
      
    }
}
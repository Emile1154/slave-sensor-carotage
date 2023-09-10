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
    upd = false;
    uint8_t inv = EEPROM.readByte(19);
    invert = false;
    if(inv == 1){
        invert = true;
    }
    uint32_t eeprom_val = EEPROM.readLong(15);
    if(eeprom_val != 4294967295){
        count = eeprom_val;
    }
    
    DDRC |= (0 << EEPROM_SIGNAL_PIN);
}
int32_t prevAngle = 0;  
int32_t angle = 0;
int32_t delta = 0;
void Encoder::updateCount(){
    angle = interface->calculateAngle(); // 0 : 360 000
    delta = angle - prevAngle;
    prevAngle = angle;
    if (delta > 180000) {
        delta -= 360000;
    } 
    else if (delta < -180000) {
        delta += 360000;
    }
    delta /= 360;
    if (invert) {
        delta *= -1;
    }
    count += delta;
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
    if(PINC & (1 << EEPROM_SIGNAL_PIN) && ! upd){
        if(EEPROM.writeLong(15,count) == true){
            upd = true;
        }
    }
}
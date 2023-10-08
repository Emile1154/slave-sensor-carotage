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
uint32_t prevAngle = 0;  
void Encoder::init(uint32_t count, uint8_t inv){
    invert = false;
    if(inv == 1){
        invert = true;
    }
    this->count = count;    
    prevAngle = interface->calculateAngle();  //fix on start
    DDRC |= (0 << EEPROM_SIGNAL_PIN);
}

uint32_t angle = 0;
float delta = 0;
void Encoder::updateCount(){
    angle = interface->calculateAngle(); // 0 : 360 000
    delta = (int32_t) (angle - prevAngle);
    prevAngle = angle;
    //correction
    if (delta > 180000) {
        delta -= 360000;
    } 
    else if (delta < -180000) {
        delta += 360000;
    }

    delta /= 360.0;
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
    if(PINC & (1 << EEPROM_SIGNAL_PIN)){
        writeFlag = true;
        
    }

    if(writeFlag){
        EEPROM.writeLong(21,count);
        while (! EEPROM.isReady());
        writeFlag = false;
    }
}
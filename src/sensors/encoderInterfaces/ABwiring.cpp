#include <sensors/encoderInterfaces/Interface.h>


ABwiring::ABwiring(){
    interface_code = 3;
}

ABwiring::~ABwiring(){

}

void ABwiring::init(){
    DDRC &= ~(1 << PIN_A);
    DDRC &= ~(1 << PIN_B);
}

uint32_t ABwiring::calculateAngle(){
    val = 0;
    if(! bit_is_clear(PINC, PIN_A)){
        val |= (1 << 1);
    }
    if(! bit_is_clear(PINC, PIN_B)){
        val |= (1 << 0);
    }
    return (uint32_t) val;
}
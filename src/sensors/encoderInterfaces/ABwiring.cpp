#include <sensors/encoderInterfaces/Interface.h>

#define PIN_A PC4
#define PIN_B PC5 

ABwiring::ABwiring(){

}

ABwiring::~ABwiring(){

}

void ABwiring::init(){
    DDRC |= (0 << PIN_A);
    DDRC |= (0 << PIN_B);
}
volatile long count = 0;
volatile int lastEncoded = 0;
volatile long encoderValue = 0;
uint32_t calculateAngle(){
    int MSB = PINC & (1 << PIN_A);
    int LSB = PINC & (1 << PIN_B);
    int encoded = (MSB << 1) | LSB;
    int sum = (lastEncoded << 2) | encoded;

    if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) {
            encoderValue++;
    } else if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) {
            encoderValue--;
    }
    count = encoderValue;
    lastEncoded = encoded;
}
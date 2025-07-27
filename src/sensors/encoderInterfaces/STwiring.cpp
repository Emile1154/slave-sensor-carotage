#include <sensors/encoderInterfaces/Interface.h>
STwiring::STwiring(/* args */)
{
    interface_code = 4;
    PCMSK1 |= (1 << PCINT13) | (1 << PCINT12);
}

STwiring::~STwiring()
{
    //interrup deinit PC
    PCMSK1 &= ~(1 << PCINT13);
    PCMSK1 &= ~(1 << PCINT12);
}

void STwiring::init(){
    DDRC &= ~(1 << PIN_A);
    DDRC &= ~(1 << PIN_B);

    //interrupt init PC

}
static uint16_t step = 7200;

static uint8_t flag1 = 0;
static uint8_t flag2 = 0;
/**
 * @brief Returns the current absolute angle in degrees from encoder data.
 *
 * The function works by checking the state of the two pins of the encoder. If pin A is low and flag1 is not set, the current angle is incremented by the step value. If pin B is high and flag1 is set, the flag is reset. If pin B is low and flag2 is not set, the current angle is decremented by the step value. If pin A is high and flag2 is set, the flag is reset.
 *
 * @return The current absolute angle in degrees.
 */
uint32_t STwiring::calculateAngle(){
    if( !(PINC & (1 << PIN_A)) && !flag1 ){
        val = val + step;
        flag1 = 1;
    }
    else if( (PINC & (1 << PIN_A)) && flag1 ){
        flag1 = 0;
    }

    if( !(PINC & (1 << PIN_B)) && !flag2 ){
        val = val - step;
        flag2 = 1;
    }
    else if( (PINC & (1 << PIN_B)) && flag2 ){
        flag2 = 0;
    }

    return val;
}

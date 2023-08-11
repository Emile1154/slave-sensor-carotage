#include <sensors/encoderInterfaces/Interface.h>

/*
MISO (DI)   PC4
CLK  (SCK)  PC5
CS   (SS)   PC3
*/

#define MISO PC4
#define CLK PC5
#define CS PC3 

void SPIwiring::init(){
    DDRC &= ~(1 << MISO); //input ,may be add pullup
    DDRC |= (1 << CS); //output
    DDRC |= (1 << CLK); //output
}

SPIwiring::SPIwiring(/* args */)
{
}

SPIwiring::~SPIwiring()
{
    //SPI.end();
}

uint32_t read24bit()
{
    uint8_t counter = 24;
    uint32_t buffer = 0;
    while (counter > 0)
    {
        PORTC |= (1 << CLK);  // Установка пина CLK в 1.
        buffer <<= 1;                // Сдвигаем переменную buffer на один бит влево.
        asm volatile("nop");
        asm volatile("nop");
        PORTC &= ~(1 << CLK);  // Установка пина CLK в 0.
        if (PINC & (1 << MISO))  // чтение по срезу
        {
            buffer |= 0x01;  // Устанавливаем младший бит буферной переменной buffer в 1.
        }
        asm volatile("nop");
        asm volatile("nop");
        counter--;                    // Уменьшаем значение счетчика на 1.
    }
    return buffer;
}

static uint8_t tableCRC6[64] = {
 0x00, 0x03, 0x06, 0x05, 0x0C, 0x0F, 0x0A, 0x09,
 0x18, 0x1B, 0x1E, 0x1D, 0x14, 0x17, 0x12, 0x11,
 0x30, 0x33, 0x36, 0x35, 0x3C, 0x3F, 0x3A, 0x39,
 0x28, 0x2B, 0x2E, 0x2D, 0x24, 0x27, 0x22, 0x21,
 0x23, 0x20, 0x25, 0x26, 0x2F, 0x2C, 0x29, 0x2A,
 0x3B, 0x38, 0x3D, 0x3E, 0x37, 0x34, 0x31, 0x32,
 0x13, 0x10, 0x15, 0x16, 0x1F, 0x1C, 0x19, 0x1A,
 0x0B, 0x08, 0x0D, 0x0E, 0x07, 0x04, 0x01, 0x02
};

/*32-bit input data, right alignment, Calculation over 18 bits (mult. of 6) */
uint8_t CRC6_43_18bit (uint32_t w_InputData){
    uint8_t b_Index = 0;
    uint8_t b_CRC = 0;

    b_Index = (uint8_t )(((uint32_t)w_InputData >> 12u) & 0x0000003Fu);

    b_CRC = (uint8_t )(((uint32_t)w_InputData >> 6u) & 0x0000003Fu);
    b_Index = b_CRC ^ tableCRC6[b_Index];

    b_CRC = (uint8_t )((uint32_t)w_InputData & 0x0000003Fu);
    b_Index = b_CRC ^ tableCRC6[b_Index];

    b_CRC = tableCRC6[b_Index];

    return b_CRC;
} 


uint32_t SPIwiring::calculateAngle(){
    PORTC &= ~(1 << CS); //set low
    uint32_t reciveData = read24bit();
    PORTC |= (1 << CS); //set hi, end reading
    return reciveData;
    //pack structure
    //  angle_data   magnet_data     crc6  
    //   0 - 13        14 - 17      18 - 23
    if(reciveData != 0){   //if data read
        bool binary[24];
        for (int i = 0; i < 24; i++) {
            binary[i] = (reciveData >> (23-i)) & 0x01;   //MSB first
        }
        uint8_t received_crc = reciveData & 0x3F;
        uint8_t calculated_crc = CRC6_43_18bit(reciveData >> 6);
        if(received_crc == calculated_crc){
            float angle;
            for(int i = 13; i >= 0  ; i--){
                angle += binary[i] << i;
            }
            return (float) angle*360/16384;
        }
        return -1; // data damaged
    }
    return -2;    //data not recived;
}


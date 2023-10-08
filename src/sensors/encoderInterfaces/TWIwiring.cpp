#include <sensors/encoderInterfaces/Interface.h>
#include <Wire.h>

TWIwiring::TWIwiring(/* args */)
{
}

TWIwiring::~TWIwiring()
{
    Wire.end();
}

void TWIwiring::init(){
    Wire.setClock(800000L); 
    Wire.begin();
}

uint16_t bytes[2];    // 0 index high byte 1 index low byte
uint32_t TWIwiring::calculateAngle(){
    Wire.requestFrom(0x06, 2, 0x03, 1, 0); //reading 2 bytes 0x03 0x04
    bytes[0] = Wire.read();
    bytes[1] = Wire.read();
    uint16_t bits_enc = (bytes[0] << 6) | (bytes[1] >> 2);
    return (uint32_t)(bits_enc*360000.0/16384.0); 
}


 //uint8_t j = 0;
    //uint32_t tm = _micros();
    //while(_micros() - tm < 1){  //0,1 ms wait threashold
    //tm = _micros();
            //bytes[j] = Wire.read();
            //j++;
            //if(j == 2 ){
            //    break;
            //}
            //}

// int address, error;
//     uint8_t nDevices;
//     for(address = 1; address < 127; address++ ){
//         Wire.beginTransmission(address);
//         error = Wire.endTransmission();
 
//         if (error == 0){
//             //Serial.print("I2C device found at address 0x");
            
//                 //Serial.print("0");
//                 //Serial.print(address,HEX);
//             return address;
//             //Serial.println(" !");
//             //nDevices++;
//         }
//     }
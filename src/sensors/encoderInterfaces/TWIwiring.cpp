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
    Wire.begin();
    Wire.setClock(400000);  //speed 400 kHz 
}

uint16_t TWIwiring::calculateAngle(){
    Wire.beginTransmission(0x06);  //default id on slave
    Wire.write(0x03);  //start address
    Wire.endTransmission();
    
    Wire.requestFrom(0x06, 2); //reading 2 bytes
    uint8_t bytes[2];    // 0 index high byte 1 index low byte
    float angle; 
    uint8_t j = 0;
    uint32_t tm = _micros();
    while(_micros() - tm < 10){  //1 ms wait threashold
        if(Wire.available()){
            tm = _micros();
            bytes[j] = Wire.read();
            j++;
            if(j == 2 ){
                break;
            }
        } 
    }
    unsigned char binary[14];    
    for (uint8_t i = 0; i < 6; i++) {
        binary[i] = (bytes[1] >> (i + 2)) & 1;
    }
    for(uint8_t i = 0; i < 9; i++){
        binary[i+6] = bytes[0] >> i & 1;   
    }
        
    //val << 1   ==   *2
    //val << 2   ==   *4
    //val << 3   ==   *8
    //val << 4   ==   *16
    for (uint8_t i = 0; i < 14; i++)
    {
        angle += (binary[i] << i);
    }
    angle = angle*3600.0/16384;
    return angle;    
}

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
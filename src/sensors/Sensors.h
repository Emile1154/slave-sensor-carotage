#pragma once
#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <sensors/encoderInterfaces/Interface.h>
#include <sensors/adc.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <IOsettings.h>
#include <EEPROMex.h>

class Tenzo
{
private:
    /* data */
    uint16_t correction;  // ADC value to kg
    uint16_t force;   //in kg
    uint8_t pwm_tension;
    uint16_t shift_adc;
    bool writeFlag = false;
public:
    void init(uint8_t _pwm, uint16_t _correction, uint16_t _adc);
    void calibrate(uint16_t weight);
    uint16_t getCorrection(); 
    void setZero();
    void updateForce();
    uint16_t getForce();
    uint8_t getPWM();
    Tenzo(); 
    ~Tenzo();
};

class Magnet
{
private:
    /* data */
    uint8_t pwm_duty;
    uint8_t magnet;
public:
    void init(uint8_t _pwm);
    uint8_t getCalibrate();
    void updateMagnet();
    void setPWM(uint8_t pwm);
    void EEPROMwrite();
    uint8_t getMagnet();
    Magnet();
    ~Magnet();
};

class Encoder
{
private:
    
    /* data */
    //float radius;  //in meter value from 0 to 1; depth = 2ПR; 
    //float depth;   // 0.01 
    Interface * interface; //I2C, SPI, AB or ABZ interface wiring
    uint32_t count = 2147483647; //0.25m = 1500; 0.5m = 3000; 1m = 6000;  
    //uint32_t totalRevolutions = 10000000;
    bool invert;
    bool writeFlag = false;
public:
    void init(uint32_t count, uint8_t inv);
    void updateCount();            //convert angle to count
    uint32_t getCount();
    bool getInvert();
    Interface * getInterface();
    void EEPROMSignalCheck();

    void setInvert(bool invert);
    void setCount(uint32_t count);
    void setInterface(Interface* Interface);
    //bool getWriteFlag();
    Encoder(); 
    ~Encoder();
};
#endif






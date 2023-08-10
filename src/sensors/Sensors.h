#pragma once
#ifndef _SENSORS_H_
#define _SENSORS_H_

#include <sensors/encoderInterfaces/Interface.h>
#include <sensors/adc.h>
#include <avr/io.h>
#include <IOsettings.h>
#include <EEPROMex.h>

class Tenzo
{
private:
    /* data */
    uint16_t correction;  // ADC value to kg
    uint16_t force;   //in kg
    volatile uint8_t pwm_tension;
    uint16_t shift_adc;
public:
    void init();
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
    bool magnet;
public:
    void init();
    uint8_t getCalibrate();
    void updateMagnet();
    void calibrate(uint8_t duty);
    bool getMagnet();
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
    uint32_t count;
    volatile uint32_t oldCount;
    uint16_t frequency;
    float totalRevolutions = 0;
    bool invert;
public:
    void init();
    void updateCount();            //convert angle to count
    void updateFrequency();

    uint32_t getCount();
    bool getInvert();
    uint16_t getFrequency();
    Interface * getInterface();
    void EEPROMSignalCheck();


    void setInvert(bool invert);
    void setCount(uint32_t count);
    void setInterface(Interface* Interface);

    Encoder(); 
    ~Encoder();
};
#endif






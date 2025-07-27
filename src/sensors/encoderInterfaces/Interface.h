#pragma once
#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include <avr/io.h>
#include <MicrosUtil.h>


#define PIN_A PC4
#define PIN_B PC5 

class Interface
{
private:
    /* data */
public:
    uint8_t interface_code = 0;
    uint32_t val = 0;
    virtual void init() = 0;
    virtual uint32_t calculateAngle() = 0;
    virtual ~Interface() noexcept = default;
};


class SPIwiring : public Interface
{

public:
    void init() override;
    uint32_t calculateAngle() override;
    SPIwiring(/* args */);
    ~SPIwiring();
};


class TWIwiring : public Interface
{
public:
    void init() override;
    uint32_t calculateAngle() override;
    TWIwiring(/* args */);
    ~TWIwiring();
};

class ABwiring : public Interface
{
public:
    void init() override;
    uint32_t calculateAngle() override;
    ABwiring(/* args */);
    ~ABwiring();
};

class STwiring : public Interface
{
public:
    
    void init() override;
    uint32_t calculateAngle() override;
    STwiring(/* args */);
    ~STwiring();
};

// class ABZwiring : public Interface
// {
// public:
//     void init() override;
//     float calculateAngle() override;
//     ABZwiring(/* args */);
//     ~ABZwiring();
// };

#endif


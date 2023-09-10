#pragma once
#ifndef _INTERFACE_H_
#define _INTERFACE_H_
#include <avr/io.h>
#include <MicrosUtil.h>

class Interface
{
private:
    /* data */
public:
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

// class ABZwiring : public Interface
// {
// public:
//     void init() override;
//     float calculateAngle() override;
//     ABZwiring(/* args */);
//     ~ABZwiring();
// };

#endif


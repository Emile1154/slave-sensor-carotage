#pragma once
#ifndef _COMMANDINTERFACE_H_
#define _COMMANDINTERFACE_H_
#include <stdint.h>
#include <InstrumentRunner.h>

class CommandInterface
{
private:
    /* data */
public:
    virtual uint16_t execute(uint8_t args[]) = 0;
    virtual ~CommandInterface() noexcept = default;
};

class CommandWrite : public CommandInterface
{
private:
    /* data */
public:
    uint16_t execute(uint8_t args[]) override;
    CommandWrite(/* args */);
    ~CommandWrite() noexcept = default;
};

class CommandRead : public CommandInterface
{
private:
    /* data */
public:
    uint16_t execute(uint8_t args[]) override;
    CommandRead(/* args */);
    ~CommandRead() noexcept = default;
};
#endif



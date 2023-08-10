#pragma once
#ifndef _MODBUS_H_
#define _MODBUS_H_
#include <modbus/CommandInterface.h>
#include <HardwareSerial.h>

class Modbus
{
private:
    /* data */
    uint8_t bufferInput[64];
    uint8_t * bufferOutput;
    uint8_t sizeBufOutput;
    uint32_t _charTimeout;
    uint8_t id;
    uint8_t counter; 
    uint8_t get_key(uint8_t hi_address, uint8_t lo_address);
    bool checkCrc();
    //uint8_t find_crc_lo_index();
    uint16_t getCrc16(uint8_t bufferArray[], uint8_t length);
    void clearBufferInput();
public:
    void init(uint16_t bodRate, uint8_t type, uint16_t timeout);
    bool bufferEmpty();
    void poll();
    uint8_t validate();
    uint8_t defineAndExecute();
    void query();
    void update(uint16_t bodRate, uint8_t type, uint16_t timeout);
    void setId(uint8_t id);
};
#endif


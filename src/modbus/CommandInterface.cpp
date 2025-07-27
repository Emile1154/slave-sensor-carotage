#include <modbus/CommandInterface.h>
CommandRead::CommandRead(/* args */)
{
}

// CommandRead::~CommandRead()
// {
// }

CommandWrite::CommandWrite(/* args */)
{
}

// CommandWrite::~CommandWrite()
// {
// }

uint16_t CommandInterface::execute(uint8_t args[]){
    return 0;
}

uint16_t CommandWrite::execute(uint8_t args[]){
    InstrumentRunner::setValue(args[0],args[1],args[2]);
    return 0;
}

uint16_t CommandRead::execute(uint8_t args[]){
    return InstrumentRunner::getValue(args[0]);
}

#include <modbus/Modbus.h>
extern const uint8_t ADDRESS_ID[2] =            {0x00, 0x03};

extern const uint8_t ADDRESS_SPEED[2] =         {0x75, 0x02};

extern const uint8_t ADDRESS_EVEN[2] =          {0x75, 0x03};

extern const uint8_t ADDRESS_INTERVAL[2] =      {0x75, 0x08};

extern const uint8_t ADDRESS_CNT_STOP_BIT[2] =  {0x75, 0x0A};


extern const uint8_t ADDRESS_ZERO_TENSION[2] =  {0x00, 0x05};

extern const uint8_t ADDRESS_CALIB_TENSION[2] = {0x00, 0x06};


extern const uint8_t ADDRESS_ENCODER_WIRE[2] =      {0x00, 0x07};

extern const uint8_t ADDRESS_INVERT_ENCODER[2] =    {0x00, 0x08};


extern const uint8_t ADDRESS_CALIB_HALL[2] =    {0x00, 0x09};



extern const uint8_t ADDRESS_GET_TENSION[2] =   {0x00, 0x0A};

extern const uint8_t ADDRESS_GET_ENCODER_HI[2] =   {0x00, 0x0B};

extern const uint8_t ADDRESS_GET_ENCODER_LO[2] =   {0x00, 0x0C};

extern const uint8_t ADDRESS_GET_HALL[2] =      {0x00, 0x0D};




const uint8_t* addresses[14] = {
  ADDRESS_ID,
  ADDRESS_SPEED,
  ADDRESS_EVEN,
  ADDRESS_INTERVAL,
  ADDRESS_CNT_STOP_BIT,

  ADDRESS_ZERO_TENSION,
  ADDRESS_CALIB_TENSION,

  ADDRESS_ENCODER_WIRE,
  ADDRESS_INVERT_ENCODER,

  ADDRESS_CALIB_HALL,

  ADDRESS_GET_TENSION,
  ADDRESS_GET_ENCODER_HI,
  ADDRESS_GET_ENCODER_LO,
  ADDRESS_GET_HALL
};

void Modbus::init(uint16_t bodRate, uint8_t type, uint16_t timeout){
  Serial.begin(bodRate, type);
  while (!Serial);
  Serial.setTimeout(2000);
  uint8_t bitsPerChar;
  if (type == SERIAL_8E2 || type == SERIAL_8O2) bitsPerChar = 12;
  else if (type == SERIAL_8N2 || type == SERIAL_8E1 || type == SERIAL_8O1) bitsPerChar = 11;
  else bitsPerChar = 10; 
  _charTimeout = (bitsPerChar * 25000) / bodRate;
}

void Modbus::setId(uint8_t id){
  this->id = id;
}

void Modbus::update(uint16_t bodRate, uint8_t type, uint16_t timeout){
  delete[] bufferOutput;
  bufferOutput = nullptr;
  Serial.end();
  Serial.begin(bodRate, type);
  Serial.setTimeout(timeout);
  while (!Serial);
  uint8_t bitsPerChar;
  if (type == SERIAL_8E2 || type == SERIAL_8O2) bitsPerChar = 12;
  else if (type == SERIAL_8N2 || type == SERIAL_8E1 || type == SERIAL_8O1) bitsPerChar = 11;
  else bitsPerChar = 10;
  _charTimeout = (bitsPerChar * 25000) / bodRate;
}

bool Modbus::bufferEmpty(){
  if(Serial.available()){
    return false;
  }
  return true;
}

void Modbus::poll(){
  uint32_t tm = _micros();
  counter = 0;     
  while(_micros() - tm <= 90 && counter < sizeof(bufferInput)){  //testing 03 03 00 08 00 01 04 2A
    if(Serial.available()){
      tm = _micros();
      bufferInput[counter] = Serial.read();
      counter++;
    }
  }
}

uint8_t Modbus::validate(){
  if(counter < 8){
    //ignore
    clearBufferInput();
    return 3;
  }
  if(bufferInput[0] != id){  // id no equals in query with slave id address
    clearBufferInput();
    return 1;       //query for not this slave device   
  }
  if(!checkCrc()){
    clearBufferInput();
    return 2;           //data damaged
  }
  return 0;
}
//uint8_t test[11] = {0x03, 0x03, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x15};
void Modbus::query(){
  for(uint8_t i = 0; i < sizeBufOutput; i++){
    Serial.write(bufferOutput[i]);   
  }
  Serial.flush();
  delete[] bufferOutput;
  bufferOutput = nullptr;
  clearBufferInput();

}

void Modbus::clearBufferInput(){
  for (uint8_t i = 0; i < 64; i++)
  {
    bufferInput[i] = 0; 
  }
}

bool Modbus::checkCrc(){
  uint8_t crc_lo_index = counter - 1;
  if(crc_lo_index <= 2){  //invalid  
    return false;
  }
  uint8_t crc_hi_index = crc_lo_index - 1;
  uint8_t tmp[crc_hi_index];
  for(uint8_t i = 0; i < crc_hi_index; i++){  //fill data in tmp array 
    tmp[i] = bufferInput[i];
  }
  uint16_t crc = getCrc16(tmp, crc_hi_index); //calc crc
  
  uint8_t crc_lo = bufferInput[crc_lo_index];
  uint8_t crc_hi = bufferInput[crc_hi_index];
  
  uint8_t crc_hi_calc = (crc >> 8) & 0xFF;
  uint8_t crc_lo_calc = crc & 0xFF;
  
  if(crc_hi != crc_hi_calc || crc_lo != crc_lo_calc){  //crc not equals
    return false;
  }
  return true; 
}

uint8_t Modbus::defineAndExecute(){ 
  CommandInterface * command = nullptr;
  uint8_t key = get_key(bufferInput[2], bufferInput[3]);
  if(bufferInput[1] == 0x06){      //write single register
    command = new CommandWrite();
    uint8_t temp[3] = {key, bufferInput[4], bufferInput[5]};
    command->execute(temp);
    delete command;
    command = nullptr;
    //generate answer 
    sizeBufOutput = 8;
    bufferOutput = new uint8_t[sizeBufOutput];
    for(uint8_t i = 0; i < 8; i++){
      bufferOutput[i] = bufferInput[i];
    }
    return 0;
  }
  if(bufferInput[1] == 0x10){      //write group register
    command = new CommandWrite();
    uint8_t count_register =   (bufferInput[4] << 8) | bufferInput[5]; 
    if(2*count_register != bufferInput[6]){ //2*count_register != count_data_bytes
      return 1;  //invalid command
    }
    for(uint8_t i = 0; i < count_register; i++){
      uint8_t temp[3] = {(uint8_t)(key+i),bufferInput[7+2*i],bufferInput[8+2*i]};
      command->execute(temp);
    }
    delete command;
    command = nullptr;

    //generate answer 
    sizeBufOutput = 8;
    bufferOutput = new uint8_t[sizeBufOutput];
    
    for(uint8_t i = 0; i < 8; i++){
      bufferOutput[i] = bufferInput[i];
    }
    uint16_t crc = getCrc16(bufferInput, 6);
    bufferOutput[6] = (crc >> 8) & 0xFF;   //hi
    bufferOutput[7] =  crc & 0xFF;         //low
    return 0;
  }
  if(bufferInput[1] == 0x03){      //single read or read group register  
    command = new CommandRead();
    uint8_t count_register =  (bufferInput[4] << 8) | bufferInput[5];

    //generate answer 
    sizeBufOutput = 5+count_register*2;
    bufferOutput = new uint8_t[sizeBufOutput];
    bufferOutput[0] = bufferInput[0];  //id
    bufferOutput[1] = bufferInput[1];  //function
    bufferOutput[2] = count_register*2; //count data bytes
    for(uint8_t i = 0; i < count_register; i++){
      uint8_t temp[1] = {(uint8_t)(key+i)};
      uint16_t val = command->execute(temp); 
      bufferOutput[3+i*2] = (val >> 8) & 0xFF; //hi
      bufferOutput[4+i*2] =  val & 0xFF;       //low
    }
    delete command;
    command = nullptr;
    uint8_t length = 3+bufferOutput[2];           
    uint16_t crc = getCrc16(bufferOutput, length);
    bufferOutput[length] = (crc >> 8) & 0xFF;   //hi
    bufferOutput[length+1] =  crc & 0xFF;       //low
    return 0;
  }
  delete command;
  command = nullptr;
  return 2;
}

uint8_t Modbus::get_key(uint8_t hi_address, uint8_t lo_address){
  for(uint8_t i = 0; i < sizeof(addresses); i++){
    if(lo_address != addresses[i][1]) {
        continue;
    }
    else if(hi_address == addresses[i][0]){
        return i;      
    }
  }
  return sizeof(addresses);
}

uint16_t Modbus::getCrc16(uint8_t bufferArray[], uint8_t length){
  unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    for (unsigned char i = 0; i < length; i++) 
    {
        temp = temp ^ bufferArray[i];
        for (unsigned char j = 1; j <= 8; j++) 
        {
            flag = temp & 0x0001;
            temp >>= 1;
            if (flag)   temp ^= 0xA001;
        }
    }
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    return temp;
}

/*
 * НПФ "Электра" программа под микроконтроллеры семейства ATmega48\88\168\328 реализующие
 * подчиненное устройство для обработки и отправки данных 
 * с трех датчиков по протоколу Modbus RTU:
 * - Датчик натяжения исполненный на тензорезистивном мосте (аналоговый сигнал и установка нуля)
 * - Датчик магнитного поля (сигнал 1/0 и смещение)
 * - Датчик расхода троса (энкодер) совместим с протоколами I2C, SPI, ST+/ST-, ABZ;
 * Разработик: Багманов Эмиль
 * Дата релиза:
 */  
#include <InstrumentRunner.h>
#include <avr/interrupt.h>
InstrumentRunner instrument;
ISR(PCINT1_vect);
int main(){
  instrument.init();
  while(1){
    instrument.run();
  }
  return 0;
}


ISR(PCINT1_vect){
    instrument.interruptEEPROM();
}

/*
 * НПФ "Электра" программа под микроконтроллеры семейства ATmega168\328 реализующие
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

void reset_interrupt_init(){
  // INT0 (PD2) usage as input
  DDRD |= (1 << PD2);
  // pull up resistor for INT0
  PORTD |= (1 << PD2);
  //setup falling edge on INT0
  EICRA |= (1 << ISC01);
  EICRA &= ~(1 << ISC00);
  //enable INT0 interrupt
  EIMSK |= (1 << INT0);
}


InstrumentRunner instrument;
ISR(PCINT1_vect);
int main(){
  instrument.init();
#if NEW_PCB_VERSION
  reset_interrupt_init();
#endif
  while(1){
    instrument.run();
  }
  return 0;
}


ISR(PCINT1_vect){
  instrument.interruptEEPROM();
}

#if NEW_PCB_VERSION
ISR(INT0_vect){
  instrument.interruptReset();
}
#endif
#include <MicrosUtil.h>
volatile uint64_t t_micros = 0;
ISR(TIMER1_COMPA_vect) {
    t_micros++; // 0.1 ms per
}

void initTimer(void){
  TCCR1A = 0; // Set Timer1 control registers to 0
  TCCR1B = 0;
  TIMSK1 |= (1 << OCIE1A); //enable compare 
  TCNT1 = 0; // Initialize the counter value to 0
  TCCR1B |= (0 << CS10) | (1 << CS11) | (0 << CS12); //div 8 
  TCCR1B |= (1 << WGM12) | (0 << WGM13); //CTC mode  
  // in CTC mode freqency_interrupt = F_CPU/(div * (OCR1A+1)) OCR1A 16bit value 0-65535 dec 
  // frequency_interrupt = 10kHz
  OCR1A = 199; 
}


uint64_t _micros(void){
  uint64_t result;
  // Ensure this cannot be disrupted
  ATOMIC_BLOCK(ATOMIC_FORCEON) {
    result = t_micros;
  }
  return result;
}

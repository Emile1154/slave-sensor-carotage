#include <sensors/adc.h>

volatile uint8_t counter;
volatile uint32_t adc_sum;

ISR(ADC_vect){ //execute om every reset bit ADSC ending conversion
    if(counter != 0){
        adc_sum += ADC;
        counter--;
        ADCSRA |= (1 << ADSC); //start convertion again
    }
}

void adc_init(void){
    ADMUX |= (1 << REFS0); // avcc
    ADMUX |= (1 << MUX2) | (1 << MUX1) | (1 << MUX0) | (0 << MUX3); // adc7 select
    ADMUX |= (0 << ADLAR);
    ADCSRA |= (1 << ADEN); //enable adc
    ADCSRA |= (1 << ADIE); //enable interrupt
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //128 div 125kHz
    
}

void adc_start(void){
    counter = 200;
    ADCSRA |= (1 << ADSC);
}
 
uint8_t adc_end(void){
    if(counter != 0){
        return 0;
    }
    return 1;
}

uint16_t adc_result(void){
    uint16_t result = adc_sum/200;
    adc_sum = 0;
    return result;
}

uint16_t readADC(void){
    adc_start();
    while(!adc_end());
    return adc_result();
}
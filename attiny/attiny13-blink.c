#define F_CPU 1200000UL
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>

#define wakeUpPeriodInSeconds 1800

volatile int watchdog_count = 0;

int awakeProcedure();

ISR(WDT_vect) {
   if (++watchdog_count > wakeUpPeriodInSeconds/8) {   // a watchdog event occurs every 8s
      watchdog_count = 0;
      awakeProcedure();
   }
}
ISR(PCINT0_vect)
{
   if (PINB & (1 << PB0)) return; // do nothing if PB0 is high

   _delay_ms(4); //debounce
   if (PINB & (1 << PB0)) return;//debounce (do nothing if PB0 is high after xx ms)

    awakeProcedure();
}


int awakeProcedure(){

   PORTB |= 1<<PB4;//switch on ESP8266
   _delay_ms(1000);//wait at least 1s

   DIDR0 &= ~(1<<PB3);

   //then wait for 5 additional seconds or until PB1 is high
   int i=0;
   for (i=0;i<50;i++)
   {
     _delay_ms(100);
     if (PINB & (1 << PB3)) break;

   }
   PORTB &= ~(1<<PB4);//switch off ESP8266
   DIDR0 |= 1<<PB3;
   return 0;

  
}

int main(void) {

   //Disable Analog to Digital Converter (ADC)
   ADCSRA &= ~(1<<ADEN);

   //Disable Analog Comparator
   ACSR = (1<<ACD);

   //Disable digital input buffers on all ADC0-ADC5 pins.
   DIDR0 = 0x3F;

   //Disable Brown-out Detector
   //Done with the fuse configuration


   // Set up Port B inputs/outputs
   DDRB   = 0b110110; 
   PORTB |= 0b001001;

   // Enable watchdog timer interrupts
   WDTCR |= (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0);
   WDTCR |= (1<<WDTIE);

   // turns on pin change interrupts
   GIMSK = 0b00100000; 
   // turn on interrupts on pins PB0  
   PCMSK = 0b00000001;


   sei(); // Enable global interrupts 

   // Use the Power Down sleep mode
   set_sleep_mode(SLEEP_MODE_PWR_DOWN);


   for (;;) {
       sleep_mode();   // go to sleep and wait for interrupt...    
   }
}

/*
Values for the watchdok interrupt
 16MS   (0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0) 
 32MS   (0<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0) 
 64MS   (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(0<<WDP0) 
 125MS  (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(1<<WDP0) 
 250MS  (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(0<<WDP0) 
 500MS  (0<<WDP3)|(1<<WDP2)|(0<<WDP1)|(1<<WDP0) 
 1S     (0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(0<<WDP0) 
 2S     (0<<WDP3)|(1<<WDP2)|(1<<WDP1)|(1<<WDP0) 
 4S     (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(0<<WDP0) 
 8S     (1<<WDP3)|(0<<WDP2)|(0<<WDP1)|(1<<WDP0) 
 */


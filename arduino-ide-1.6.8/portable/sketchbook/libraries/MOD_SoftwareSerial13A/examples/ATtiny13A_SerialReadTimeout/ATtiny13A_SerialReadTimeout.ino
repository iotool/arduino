// ATtiny13A_SerialReadTimeout.ino

#define SERIAL_BEGIN   9600
#define SERIAL_TX      PB0
#define SERIAL_RX      PB1
#define SERIAL_BUFFER  12
#include <SoftwareSerial13A.h>

#define F_CPU 1200000UL      // 1.2 MHz
#include <avr/interrupt.h>   // timer interrupt
#include <util/delay.h>      // low level delay
#include <avr/io.h>

#define STATE_DEFAULT  0     // ignore timeout
#define STATE_READ     1     // start of timeout
#define STATE_TIMEOUT  2     // end of timeout

volatile uint16_t g_timer_counter = 0; // 65.535
volatile uint8_t  g_timer_state = STATE_DEFAULT;

ISR(TIM0_OVF_vect) {
  g_timer_counter++;
  if (g_timer_state == STATE_READ) {
    g_timer_state = STATE_TIMEOUT;
    // clear io register to skip "WaitStart" at uart_getc2
    char c;
    __asm volatile(
    " cbi %[uart_port]-2, %[uart_pin] \n\t"
    : "=r" (c)
    : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
      [uart_pin] "I" (UART_RX)
    );
  }
}

void lf() {
  SERIAL_WRITE2('\n');
}

int main(void) 
{
  // pinout PB__543210
  PORTB |=  0b00000001; // HIGH
  DDRB  |=  0b00000001; // OUTPUT

  // timer
  GTCCR |= (1 << TSM);                                  // stop timer
  TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // clear prescale
  TCCR0B |= ((1 << CS01) | (1 << CS00));                // prescale F_CPU/64 13,65ms = 64*256*0,833us
  TCNT0 = 0;                                            // clear counter
  GTCCR &= ~(1 << TSM);                                 // start timer
  TIMSK0 |= _BV(TOIE0);                                 // enable timer overflow interrupt = 13,65ms
  asm("sei");                                           // enable interrupt

  // boot message
  SERIAL_PRINTS2("RUN"); lf(); _delay_ms(60);

  while(1) 
  {
    // ready for input message
    TCNT0 = 0;
    SERIAL_PRINTS2("CMD"); lf(); 
  
    // read serial input
    TCNT0 = 0;
    g_timer_state = STATE_READ;
    while (g_timer_state == STATE_READ) 
    {
      SERIAL_READLN2;
      TCNT0 = 0;
      if (g_timer_state == STATE_TIMEOUT) {
        SERIAL_PRINTS2("NOP"); lf(); 
        _delay_ms(60);
      } else {
        g_timer_state = STATE_DEFAULT;
      }
    }
    g_timer_state = STATE_DEFAULT;
  
    // output data
    TCNT0 = 0; 
    SERIAL_PRINTS2("DAT"); lf(); 
    _delay_ms(60);
    TCNT0 = 0; 
    SERIAL_PRINTN2(g_timer_counter); lf();

    // delay
    _delay_ms(1200); // 1000 ms delay
  }
}

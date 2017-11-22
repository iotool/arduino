// https://github.com/iotool/arduino/tree/master/arduino-ide-1.6.8/portable/sketchbook/attiny13a/ATtiny13A_Serial_CoMCU
// 
// Provide low current cases by cheap ATtiny13A microcontroller with serial UART communication.
// 
// This sketch implements a template for serial communication and power reduction based on ATtiny13A.
// The ATtiny used as a secondary slave in combination with a primary master microcontroller.
// During power off or deep sleep of the master, the ATtiny messures sensors, keep data and clock.
// On events or after counter overflow the ATtiny wake up or power on the master and send its data.
// 
//   begin    > "\n"                    ignore garbage
//   version  > "C:1\n"                 api "C" (version 1)
//   request  < "B=OHR\n"               set buffer[2]=248
//   response > "E!\n"                  on error (optional)
//   reponse  > "4:0000000000@A@A\n"    segment 4 with parity byte
//   reponse  > "3:00000000000000\n"    segment 3
//   reponse  > "2:00000000000000\n"    segment 2
//   reponse  > "1:00000000000000\n"    segment 1
//   sleep...                           power off
// 
// Two gpio pins are used for serial communication between ATtiny and the master microcontroller.
// The data on the ATtiny are reset save to use the reset pin as push button without fuse bits.
// Tree pins are left for sensor or actor usage.
// 
// 2017-11-22  init 0x00, comments [678 byte program / 29 byte sram]

// --- arduino ide libraries ---

#include <avr/io.h>                // pinmode
#include <util/delay.h>            // wait milliseconds
#include <avr/interrupt.h>         // interrupt main
#include <avr/sleep.h>             // reduce power
#include <util/crc16.h>            // crc hash

// --- configuration ---

#define F_CPU          1200000UL   // 1.2 MHz for delay

#define UART_BAUDRATE  19200       // bits per second
#define UART_TX        PB0         // TXD serial output
#define UART_RX        PB1         // RXD serial input
#define UART_BUFFER    6           // RXD command buffer

#define DATA_BUFFER    28          // data structure
#define DATA_SEGMENT   7           // serial output partition
#define DATA_BEGIN     0           // ----- CRC begin
#define DATA_END       25          // ----- CRC end
#define DATA_CRC_OLD   26          // crc old value
#define DATA_CRC_NEW   27          // crc new value

// --- features ---

static char uart_getchar();
static void uart_putchar(char c);
static void uart_puthex(char n);
    uint8_t dataBufferGetCrc(uint8_t pos, uint8_t val);
       void dataBufferSet(uint8_t pos, uint8_t val);
       void dataBufferIncrement(uint8_t counter);
static void dataBufferInit();
static void powerInit();
static void powerReduce();
       void serialPrintLf();
static void serialPrintVersion();
static void serialPrintError();
static void serialPrintDataBufferSegment(uint8_t segment);
static void serialPrintDataBuffer();
       void serialReadln();
static void timerInit();
       void timerNextInterval();

// --- variables ---

volatile uint8_t                   // use variable inside of interrupt
  g_timerFlags = 0b00000000        // enable/disable timer features
;

volatile uint8_t                   // use variable inside of interrupt
  g_dataBuffer[DATA_BUFFER]        // serial shared memory buffer
  __attribute__ ((section (".noinit")))
;

// === main program ===

int main(void) 
{
  // ---------- Setup ----------

  // The setup code execute once at startup or after reset button.
  // At power on the crc hash is invalid and the data initialized with 0.
  // After reset the crc hash is valid and the data don't initialized again.
 
  dataBufferInit();                // initialize buffer
  timerInit();                     // initialize timer
  powerInit();                     // initialize sleep

  while(1)
  {
    // ---------- Loop ----------

    // The loop code executes endless until reset or power off.
    // At the end of each loop the ATtiny goto deep sleep mode to reduce current.
    // Each communication time slot interrupts after 13 ms to ensure power off.

    timerNextInterval();           // wait for next timer interval
    serialPrintLf();               // ensure end of prev communication
    serialPrintVersion();          // send api version identify to client
    serialPrintLf();               // split serial output by new line

    g_timerFlags |= 0b00000001;    // enable timeout for readln by interrupt
    serialReadln();                // read serial command line
    g_timerFlags = 0;              // disable timeout by interrupt
    
    serialPrintDataBuffer();       // send serial output buffer
    DDRB &= ~(1 << UART_TX);       // tx output pin as input to turn led off

    powerReduce();                 // power down
  }
}

// === sub routines ===

// --- timer interrupt ---

ISR(TIM0_OVF_vect)
{
  // This interrupt will execute at each timer counter overflow.
  // The 8 bit counter incremented by each clock cylcle of 0,833 us (1000ms/1,2 MHz).
  // After 256 iterations the overflow reached every 213,25 us (prescale 1).
  // With prescale 64 the overflow exec every 13,65 ms.
  
  if (g_timerFlags & 0b00000001) {
    // Interrupt serial read after timeout of 13,65 ms.
    // The original serial read wait endless for a input impulse signal.
    // So the ATtiny could not goto power save mode, if the master don't send any request.
    // To ensure power safe mode the endless loop stopped by this fake impulse.
    __asm volatile(
      " cbi %[uart_port], %[uart_pin] \n\t"    // interrupt rx pin endless start loop
      :
      : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
        [uart_pin] "I" (UART_RX)
    );  
  }
}

static // -4 ByteCode
void timerInit()
{
  // Initialize interrupt interval with CS prescale 64.
  // The counter overflow will reached every 13,65 ms.

  //-# GTCCR |= (1 << TSM); #-                          // global stop all timers
  TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00)); // prescale remove
  TCCR0B |= ((1 << CS01) | (1 << CS00));                // prescale F_CPU/64  13,65ms = 64*256*0,833us
  //-# TCNT0 = 0; #-                                    // clear TIM0_OVF counter
  //-# GTCCR &= ~(1 << TSM); #-                         // global start all timers
  TIMSK0 |= _BV(TOIE0);                                 // define prescale for 13,65ms OVF
  asm("sei");                                           // enable all interrupts
}

void timerNextInterval()
{
  // The timing for serial communication is very sensitive.
  // This procedure wait until timer overflow to to prevent communication errors.
  
  while(TCNT0>1) {                                      // next interval
    //-# asm("nop"); #-
  }
}

// --- power down ---

static // -6 ByteCode
void powerInit() // +24 ByteCode
{
  // To reduce current consumption the ATtiny goto deep sleep mode.
  // The sleep interval define by the WDP prescale.
  
  WDTCR |= (0<<WDP3)|(0<<WDP2)|(1<<WDP1)|(1<<WDP0); // define sleep interval of 125 ms
  WDTCR |= (1<<WDTIE); WDTCR |= (0<<WDE); sei(); set_sleep_mode(SLEEP_MODE_PWR_DOWN); // define power down sleep mode
}

static // -6 ByteCode
void powerReduce() // +12 Byte
{
  // During sleep mode the cpu turned off and only watchdog counter increments by the clock.
  // After overflow of the watchdog counter the code will continue at this point.
  
  sleep_mode(); // sleep now
}

// --- data buffer ---

uint8_t dataBufferGetCrc(uint8_t pos, uint8_t val)      // calculate crc-hash
{
  // Volatile data will not initialized and has random data at power on or reset.
  // CRC checksum provide reset save data by updating data and double checksum.
  // This function can calculate the new CRC for new data without update.

  uint8_t crc = 0, i;
  for (i=DATA_BEGIN; i<=DATA_END; i++) {
    if (i == pos) // use new value for crc
      crc = _crc8_ccitt_update(crc,val);
    else // use existing value for crc
      crc = _crc8_ccitt_update(crc,g_dataBuffer[i]);
  }
  return crc;
}

void dataBufferSet(uint8_t pos, uint8_t val)            // calculate crc-hash for new value
{
  // To ensure reset save updates this procedure uses tree steps.
  // First the new CRC calculates and set, second the data and third the old CRC.
  // If there will be an reset during one step the old or new CRC will be valid.
  // The master microcontroller can verify both CRC.
  
  uint8_t crc = dataBufferGetCrc(pos,val);              // calc hash
  g_dataBuffer[DATA_CRC_NEW] = crc;                     // set new value to crc
  g_dataBuffer[pos] = val;                              // set new to data
  g_dataBuffer[DATA_CRC_OLD] = crc;                     // set old value to crc
}

void dataBufferIncrement(uint8_t counter)               // increment data and hash
{  
  // Increment a value at address and recalculate the CRC hash.
  
  dataBufferSet(counter,g_dataBuffer[counter]+1);
}

static // -4 ByteCode
void dataBufferInit()                                   // initialize buffer
{ 
  // At power on the memory is not initialized and filled with random values.
  // This procedure verify the old and the new CRC hash during startup phase.
  // If both CRC hashes are invalid, the memory will be initialized.
  
  uint8_t crc = dataBufferGetCrc(DATA_BUFFER,0),i;      // calculate crc
  if (crc != g_dataBuffer[DATA_CRC_OLD]) {              // validate old crc vs. crc
   if (crc != g_dataBuffer[DATA_CRC_NEW]) {             // validate new crc vs. crc
      for (i=DATA_BUFFER-1; i>0; i--) {                 // initialize ..
        g_dataBuffer[i] = 0x00;                         // .. buffer
      }
      dataBufferSet(0,0);                               // calculate hash
    }
  }
}

// --- serial communication ---

void serialPrintLf()                                    // end of line
{
  // Each communication response and request are separated by line feed.
  // The line feed used by serial client to initialize the read buffer.
  
  uart_putchar('\n');
}

static // -2 ByteCode
void serialPrintVersion()                               // api version
{
  // The version identifier define the communication protocol and address mapping.
  // By this ID the master can use different versions of ATtiny firmware.
  
  uart_putchar('C'); // "C:1" ~ Controller Version 1
  uart_putchar(':');
  uart_putchar('1');
}

static // -2 ByteCode
void serialPrintError()                                 // command error
{
  // The ATtiny send an error to the master for each wrong request.
  // If there is no error response, the request was successful.
  
  uart_putchar('E'); // "E!" ~ Error
  uart_putchar('!');
}

static // -66 ByteCode 
void serialPrintDataBufferSegment(uint8_t segment)      // output segment
{
  // Each serial output limited by the timer overflow interval.
  // The full buffer partitioned into smaller segments.
  
  uart_putchar(segment+48);                             // ascii code for segment
  uart_putchar(':');                                    // split segment and data
  for (uint8_t i=DATA_SEGMENT; i>0; i--) {
    uart_puthex(g_dataBuffer[(segment*DATA_SEGMENT)-i]);
  }
  serialPrintLf();
}

static // -32 ByteCode 
void serialPrintDataBuffer()                            // output buffer
{
  // This procedure send the data segments to the master.
  // The number of segments and the transfered data could reduced by a parameter.
  
  for (uint8_t i=(DATA_BUFFER/DATA_SEGMENT); i>0; i--) {
    timerNextInterval();
    serialPrintDataBufferSegment((uint8_t)i);
  }
}

void serialReadln()                                     // read line
{
  // The readln procedure implements the request and response feature.
  // The input buffer filled by the serial input, parsed and verified.
  // If the parity hash is ok, the data will be set. 
  // On invalide requests an error will response to the master.
  
  // --- read serial input into buffer or interrupt by timeout
  char *ptrSerialRead, serialBuffer[UART_BUFFER];
  ptrSerialRead = serialBuffer; 
  while((*(ptrSerialRead++)=uart_getchar())!='\n' && (ptrSerialRead-serialBuffer)<UART_BUFFER);

  // --- parse, validate input and set data
  // The request codes optimized for minimal programm memory and simple bitwise operations.
  // Char 64 represents the begin of the range and is equivalent to 0.
  // The parity byte and hex code should prevent most communication issues (Addresse=HighLowParity).
  // [@..Y] = [@..O][@..O][@.._] \n
  uint8_t pos = serialBuffer[0]^0x01000000; // 0..25
  uint8_t val = ((serialBuffer[2]^0x01000000)<<4)+(serialBuffer[3]^0x01000000); // 0..255
  uint8_t crc = (serialBuffer[0]^serialBuffer[1]^serialBuffer[2]^serialBuffer[3]^serialBuffer[5]^0x00100000); // 64..95

  if ((pos < DATA_BUFFER) && (crc == serialBuffer[4])) {
    dataBufferSet(pos,val);
  } else {
    serialPrintError();
    serialPrintLf();
  }
}

// --- uart serial communication ---

// Copyright (c) 2016, Lukasz Marcin Podkalicki <lpodkalicki@gmail.com>
// ATtiny13/008 Example of Software UART.
// https://raw.githubusercontent.com/lpodkalicki/blog/master/avr/attiny13/008_software_uart/main.c
// http://blog.podkalicki.com/attiny13-software-uart-debug-logger/

#ifndef UART_RX_ENABLED
# define   UART_RX_ENABLED (1)   // Enable UART RX
#endif /* !UART_RX_ENABLED */

#ifndef UART_TX_ENABLED
# define   UART_TX_ENABLED (1)   // Enable UART TX
#endif /* !UART_TX_ENABLED */

#ifndef F_CPU
# define   F_CPU    (1200000UL)  // 1.2 MHz
#endif  /* !F_CPU */

#if defined(UART_TX_ENABLED) && !defined(UART_TX)
# define   UART_TX         PB0   // Use PB0 as TX pin
#endif  /* !UART_TX */

#if defined(UART_RX_ENABLED) && !defined(UART_RX)
# define   UART_RX         PB1   // Use PB1 as RX pin
#endif  /* !UART_RX */

#if (defined(UART_TX_ENABLED) || defined(UART_RX_ENABLED)) && !defined(UART_BAUDRATE)
# define   UART_BAUDRATE   (19200)
#endif  /* !UART_BAUDRATE */

#define UART_TXDELAY    (int)(((F_CPU/UART_BAUDRATE)-7 +1.5)/3)
#define UART_RXDELAY    (int)(((F_CPU/UART_BAUDRATE)-5 +1.5)/3)
#define UART_RXDELAY2   (int)((UART_RXDELAY*1.5)-2.5)
#define UART_RXROUNDED  (((F_CPU/UART_BAUDRATE)-5 +2)/3)

#if UART_RXROUNDED > 127
# error Low baud rates unsupported - use higher UART_BAUDRATE
#endif

char uart_getchar(void)
{
#ifdef  UART_RX_ENABLED
  char c;
  uint8_t sreg;

  // Serial read byte with UART protocol. If there is no data, 
  // the read terminated after timer interrupt interval  by a fake impulse.

  sreg = SREG;
  //-# cli(); #- don't disable timer to interrupt after timeout
  PORTB &= ~(1 << UART_RX); // "low" - disable internal pullup resistor
  DDRB &= ~(1 << UART_RX);  // "input" - pin as input to get serial data

  __asm volatile(
    " ldi r18, %[rxdelay2] \n\t" // 1.5 bit delay
    " ldi %0, 0x80 \n\t" // bit shift counter
    "WaitStart: \n\t"
    " sbic %[uart_port]-2, %[uart_pin] \n\t" // wait for start edge (Skip if Bit in I/O Register is Cleared)
    " rjmp WaitStart \n\t"
    "RxBit: \n\t"
    // 6 cycle loop + delay - total = 5 + 3*r22
    // delay (3 cycle * r18) -1 and clear carry with subi
    " subi r18, 1 \n\t"
    " brne RxBit \n\t"
    " ldi r18, %[rxdelay] \n\t"
    " sbic %[uart_port]-2, %[uart_pin] \n\t" // check UART PIN
    " sec \n\t"
    " ror %0 \n\t"
    " brcc RxBit \n\t"
    "StopBit: \n\t"
    " dec r18 \n\t"
    " brne StopBit \n\t"
    : "=r" (c)
    : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
    [uart_pin] "I" (UART_RX),
    [rxdelay] "I" (UART_RXDELAY),
    [rxdelay2] "I" (UART_RXDELAY2)
    : "r0","r18","r19"
  );
  
  SREG = sreg;
  return c;
#else
  return (-1);
#endif /* !UART_RX_ENABLED */
}

void uart_putchar(char c)
{
#ifdef  UART_TX_ENABLED
  uint8_t sreg;

  // Serial write byte with UART protocol. The original source was modifed 
  // to use timer for clock and interrupts.

  sreg = SREG;
  //-# cli(); #- don't disable timer to implement clock
  PORTB |= 1 << UART_TX; // "high" - no signal impulse
  DDRB |= 1 << UART_TX;  // "output" - pin as output to send serial data

  __asm volatile(
    " cbi %[uart_port], %[uart_pin] \n\t" // start bit (Clear Bit in I/O Register)
    " in r0, %[uart_port] \n\t"
    " ldi r30, 3 \n\t" // stop bit + idle state
    " ldi r28, %[txdelay] \n\t"
    "TxLoop: \n\t"
    // 8 cycle loop + delay - total = 7 + 3*r22
    " mov r29, r28 \n\t"
    "TxDelay: \n\t"
    // delay (3 cycle * delayCount) - 1
    " dec r29 \n\t"
    " brne TxDelay \n\t"
    " bst %[ch], 0 \n\t"
    " bld r0, %[uart_pin] \n\t"
    " lsr r30 \n\t"
    " ror %[ch] \n\t"
    " out %[uart_port], r0 \n\t"
    " brne TxLoop \n\t"
    :
    : [uart_port] "I" (_SFR_IO_ADDR(PORTB)),
      [uart_pin] "I" (UART_TX),
      [txdelay] "I" (UART_TXDELAY),
      [ch] "r" (c)
    : "r0","r28","r29","r30"
  );

  SREG = sreg;
#endif /* !UART_TX_ENABLED */
}

void uart_puthex(char n)
{
  // Send hexadecimal output format to improve the parity sensibility.
  // The ascii codepage normed for 7 bit data for any platform.
  
  if(((n>>4) & 15) < 10)
    uart_putchar('0' + ((n>>4)&15));
  else
    uart_putchar('A' + ((n>>4)&15) - 10);
  n <<= 4;
  if(((n>>4) & 15) < 10)
    uart_putchar('0' + ((n>>4)&15));
  else
    uart_putchar('A' + ((n>>4)&15) - 10);
}


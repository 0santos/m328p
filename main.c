#define F_CPU 16000000L
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <LiquidCrystal.h>

const int rs = 13, en = 12, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

volatile int encoder0Pos = 0;

// Declaração de subrotinas
void initinterrupts(void);
void inittimer0(void);

// The Interrupt Service Routine for external INT1
ISR(INT1_vect)
{
  if (PIND & _BV(PD2))
  {
    encoder0Pos++;
  }
  else if (!(PIND & _BV(PD2)))
  {
    encoder0Pos--;
  }
}

// Timer0 compare A interrupt 2kHz
ISR(TIMER0_COMPA_vect)
{
  TCNT0  = 0; // Reset the counter first
}

// 8bit Timer
void inittimer0()
{
  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);
}

void initinterrupts()
{
  // Inputs pins
  DDRD &= ~(1<<PD2);
  DDRD &= ~(1<<PD3);
  
  // Enable the internal pull-up resistors
  PORTD |= (1<<PD2)|(1<<PD3);
  
  // Falling edge in INT1 (PD3) to cause interrupt
  EICRA |= (1<<ISC11);
  
  // Enable on INT1
  EIMSK |= (1<<INT1);
  
  sei(); //allow interrupts
}

int main()
{
  initinterrupts();
  inittimer0();
  
  lcd.begin(16, 2);
  lcd.print("Encoder test");
  
  while(1)
  {
    lcd.setCursor(0, 1);
    lcd.print(encoder0Pos);
    
    _delay_ms(10);
  }
}
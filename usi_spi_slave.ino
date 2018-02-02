//========================================================================
// usi_spi_slave.ino - simple gpio communication for attinyXX MCU
//------------------------------------------------------------------------
// Copyright (c) 2017-2018 Ji Wong Park <sailfish009@gmail.com>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

////////////////////////////////////////////////////////////////////////////

#define CS   PA6         // MOSI    // PA6    // Serial DATA  INPUT
#define CLK  PA4         // SCK     // PA4    // Serial Clock INPUT

#define P1   6           // MOSI    // PA6    // Serial DATA  INPUT
#define P2   5           // MISO    // PA5    // Serial DATA  OUTPUT
#define P3   4           // SCK     // PA4    // Serial Clock INPUT

////////////////////////////////////////////////////////////////////////////

volatile byte sid = 0;   // serial bit packet id
volatile byte index = 0;
volatile byte write_index = 0;
byte buf[4] = {0};       // 4 byte
bool up = 0;             // 1: clock rising,  0: clock falling

////////////////////////////////////////////////////////////////////////////

void setup() 
{
  // disable interrupts
  cli();
  pinMode(P1, INPUT);
  pinMode(P2, OUTPUT);
  pinMode(P3, INPUT);

  USICR = (1<<USIWM0)|(1<<USICS1);

  PCMSK0|=1<<CS; 
  PCMSK0|=1<<CLK;
  GIMSK|=1<<PCIE0;

  delay(500);
  // enable interrupts
  sei();
}


// CLOCK PIN
ISR(PCINT4_vect)
{ 
  switch( (PINA & (1<<CLK)) )
  {
    case 0:
    sid = 0;
    index = 0;
    USICR |= (1<<USIOIE);
    USISR = 1<<USIOIF;      
    up = 0;
    break;

    default:
    USICR &= ~(1<<USIOIE);
    up = 1;
    break;
  }
}


// DATA PIN
ISR(PCINT6_vect)
{ 
  switch( (PINA & (1<<CS)) )
  {
    case 0:      // data : LOW
      if(up)     // only receive data when clock is rising
      {
        USIDR &= ~(1<<write_index);
        ++write_index;
      }
      else       // only send data when clock is falling
      {
        digitalWrite(P2,  LOW);
      }
      break;

    default:     // data : HIGH
      if(up)     // only receive data when clock is rising
      {
        USIDR |= 1<<write_index;
        ++write_index;
      }
      else       // only send data when clock is falling
      {
        digitalWrite(P2,  HIGH);
      }
      break;
  }
}


ISR(USI_OVF_vect)
{ 
  write_index = 0;
   
  switch(sid)
  {
  case 0:               
    sid   = USIDR;      
    USISR = 1<<USIOIF;  
    break;
    
  default:
    USIDR = sid;
    USISR = 1<<USIOIF;
    break;
  }      
}


void loop() 
{
  // USE ISR().
}

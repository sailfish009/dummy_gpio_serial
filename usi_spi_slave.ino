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

#define P1   6           // MOSI    // PA6    // Serial DATA  INPUT
#define P2   5           // MISO    // PA5    // Serial DATA  OUTPUT
#define P3   4           // SCK     // PA4    // Serial Clock INPUT
#define CS   PA0         // CS      // PA0    // manual interrupt

////////////////////////////////////////////////////////////////////////////

volatile byte old_USIDR = 0;                  // USIDR save
volatile up = 0;                              // 1: clock rising (read),  0: clock falling (write)
volatile byte count = 0;
static volatile byte index = 0;               // read write index
static volatile byte buffer[4] = {0};         // 4 byte

////////////////////////////////////////////////////////////////////////////

inline byte reverse (byte x) 
{
  x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
  x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
  x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);
  return x;
}

void setup() 
{
  // disable interrupts
  cli();
  pinMode(0, INPUT);
  pinMode(P1, INPUT);
  pinMode(P2, OUTPUT);
  pinMode(P3, INPUT);

  USICR = (1<<USIWM0)|(1<<USICS1);
  PCMSK0|=1<<CS; 
  GIMSK|=1<<PCIE0;

  delay(500);
  // enable interrupts
  sei();
}


// manual interrupt when function call
ISR(PCINT0_vect)
{ 
  switch( (PINA & (1<<CS)) )
  {
    // read
    case 0:
    USICR &= ~(1<<USIOIE);
    up = 1;
    count = 0;
    USICR |= (1<<USIOIE);
    USISR = (1<<USIOIF);
    break;

    // write
    default:
    USICR &= ~(1<<USIOIE);
    USIDR = 0; 
    up = count = index = 0;
    USICR |= (1<<USIOIE);
    USISR = (1<<USIOIF);
    break;
  }
}


ISR(USI_OVF_vect)
{ 
  USISR = (1<<USIOIF);
  
  switch(up)
  {
    case 0:
    old_USIDR = USIDR;
    if(count == 0) 
    {
      index = old_USIDR; // & 0x03;  //buffer[4]
      count = 1;
    }
    else
    { 
      buf[reverse(index)&0x03] = old_USIDR;
      count = 0;
    }
    break;

    default:
    count = 0;
    USIDR = buffer[reverse(index)&0x03];
    break;
  }    
}


void loop() 
{
  // USE ISR().
}

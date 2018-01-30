//========================================================================
// dummy_gpio_serial.ino - simple gpio communication for attinyXX MCU
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


#define PIN_OUT(x)  (DDRX  |=  ( 1 << x))
#define PIN_IN(x)       (DDRX  &= ~( 1 << x))

#define LED_OFF(x)  (PORTX |=  (1<< x))
#define LED_ON(x)    (PORTX &= ~(1<< x))

#define READ      digitalRead
#define WRITE     digitalWrite

#define P1        1       // MOSI        // INPUT 
#define P2        2       // MISO         // INPUT
#define P3        3       // SEL            // OUTPUT
#define P4        4       // SCL            // OUTPUT
#define P5        5       // LED

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte val = 0;
int  count = 0;
int  ret_count = 0;
bool d_ready = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() 
{
  // INPUT
  PIN_IN(P1);   PIN_IN(P2);  

  // OUTPUT
  PIN_OUT(P3);  PIN_OUT(P4);  
  
  // LED
  PIN_OUT(P5);
}

void loop()
{
  // INPUT
  if( READ(P1) )
  {
    // PON(Pin); // ON  // READ
    if(d_ready)
    {
      d_ready = 0;
	  if(count == 0) val = 0;
      val |= (READ(P2) << count);
      if(++count >7) 
	  { 
	    count = 0; 
	    //  OUTPUT
        //////////////////////////////////////////////////////////
		delay(1000);
		for(int i =0; i<8; ++i)
		{
			WRITE(P3, HIGH);
			delay(30);
			WRITE(P4, !((val>>i)&0x1));
			WRITE(P3, LOW); 
			delay(30);
		}
		val = 0;
        //////////////////////////////////////////////////////////
	  }
    }
  }
  else
  {
    d_ready = 1;
  }
  //////////////////////////////////////////////////////////
}







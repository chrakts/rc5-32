#include "rc5-32.h"
#include "Serial.h"
extern Serial debug;


extern volatile uint8_t IR_Remote,Taste_Neu;
// angepasst an 4 MHz Oszillator und normalen Signal
// Signal an Interrupt 1

static unsigned char status=0,bitcount=0;
static unsigned char  data=0,data1,data2,Taste_alt;

ISR ( RC5_INTVEC )     /* signal handler for external interrupt int1 */
{
unsigned char error;
uint16_t zeit;

	error=false;
	switch( status )
	{
		case 3:
			zeit = RC5_TIMER.CNT;
			RC5_TIMER.CNT = 1;
			data = data >> 1;
			if( zeit<ZEIT_BIT_LANG )
      {
				data = data | 0x80;
      }
			bitcount = bitcount + 1;

			switch(bitcount)
			{
				case 9:  data1=data; data=0 ; break;
				case 17: data2=data; data=0 ; break;
			}
			if(bitcount>=17)
			{
				RC5_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc;    //TIMSK0 = TIMSK0 & (~ _BV(TOIE0));
				data1=~data1;
				data2=~data2;
				if( data1==67 )
				{
					if( data2 == Taste_alt )
					{
						IR_Remote=data2;
					}
					Taste_alt = data2;
				}
				status=0;
				bitcount=0;
				falling_edge();
			}
		break;
		case 2:
			zeit = RC5_TIMER.CNT;
			RC5_TIMER.CNT=1;
			data=0;
			bitcount=0;
			if( zeit>ZEIT_START_HIGH )		//
			{
				RC5_TIMER.CTRLA = TC_CLKSEL_DIV256_gc;		// clocked at CK/256, 16Mhz Quarz
				RC5_TIMER.CNT = 1;
				status=3;			// neue Taste gedrückt
				Taste_Neu = true;
				rising_edge();
			}
			else
				error=true;
		break;
		case 1:
			zeit = RC5_TIMER.CNT;
			if( zeit>ZEIT_START_LOW )
			{
				RC5_TIMER.CNT = 1;
				status = 2;
				falling_edge();
			}
			else
			{
				RC5_TIMER.CTRLA = TC_CLKSEL_DIV256_gc;		// clocked at CK/256, 16Mhz Quarz
				RC5_TIMER.CNT = 1;
				status = 3;				// Taste gedückt gehalten, Startsequenz fehlt
				Taste_Neu = false;
				rising_edge();
				bitcount = 1;			// erstes Byte wurde übersprungen
			}
		break;
		case 0:
			RC5_TIMER.CTRLA = TC_CLKSEL_DIV256_gc;			// clocked at CK/1024, 16Mhz Quarz
			RC5_TIMER.CNT = 1;
			data = 0L;
			status = 1;
			rising_edge();
			RC5_TIMER.INTFLAGS = TC0_OVFIF_bm; //   TIFR0 = TIFR0 | TOV0;  // Flag zurücksetzen
			RC5_TIMER.INTCTRLA = TC_OVFINTLVL_LO_gc;    //TIMSK0 = TIMSK0 | _BV(TOIE0);
		break;
		default: error=true;
	}
	if( error==true )
	{
		RC5_TIMER.CTRLA = TC_CLKSEL_DIV256_gc;			// clocked at CK/1024, 16Mhz Quarz
		status = 0;
		bitcount = 0;
		RC5_TIMER.CNT = 1;
		RC5_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc;    // TIMSK0 = TIMSK0 & (~ _BV(TOIE0));
		falling_edge();
	}
}

void rc5_init( void )
{
	//TCCR0B = CLK256;		// Timer/Counter 0 clocked at CK/1024, 16Mhz Quarz
	RC5_TIMER.CTRLA  = TC_CLKSEL_DIV256_gc;
	RC5_PORT.RC5_INTMASK = 0;                   // Interrupt ausmaskieren
	falling_edge();
	RC5_PORT.INTCTRL &= RC5_INT_OFF;            // Interruptpriorität zurücksetzen
	RC5_PORT.INTCTRL |= RC5_INT_LEVEL;          // Interruptpriorität setzen
	RC5_PORT.RC5_INTMASK = RC5_PIN;             // Interrupt einmaskieren
}

SIGNAL(RC5_SIGNAL_OVF)     /* signal handler for counter 0 overflow */
{
	RC5_TIMER.CTRLA = TC_CLKSEL_DIV256_gc;		   // Timer/Counter 0 clocked at CK/1024, 16Mhz Quarz
	status = 0;
	bitcount = 0;
	RC5_TIMER.INTCTRLA = TC_OVFINTLVL_OFF_gc;    //TIMSK0 = TIMSK0 & (~ _BV(TOIE0));

}

void falling_edge( void )
{
  RC5_PORT.RC5_INTMASK = 0;                       // Interrupt ausmaskieren   EIMSK = EIMSK & (~_BV(INT1));
  RC5_PORT.RC5_PINCTRL = PORT_OPC_PULLUP_gc | PORT_ISC_FALLING_gc;     //EICRA = ( EICRA & (~_BV(ISC10))) | _BV(ISC11);
	RC5_PORT.RC5_INTMASK = RC5_PIN;                 // Interrupt einmaskieren  EIMSK = EIMSK | _BV(INT1);
}

void rising_edge( void )
{

	RC5_PORT.RC5_INTMASK = 0;                       // Interrupt ausmaskieren   EIMSK = EIMSK & (~_BV(INT1));
	RC5_PORT.RC5_PINCTRL = PORT_OPC_PULLUP_gc | PORT_ISC_RISING_gc;     //EICRA =  EICRA | _BV(ISC10) | _BV(ISC11);
	RC5_PORT.RC5_INTMASK = RC5_PIN;                 // Interrupt einmaskieren  EIMSK = EIMSK | _BV(INT1);
}


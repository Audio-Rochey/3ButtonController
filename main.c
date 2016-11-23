#include "msp430.h"
#include "stdbool.h"
int t;

void variablearraytoflash(void);

#pragma DATA_SECTION(nonvolatilecoefficients, ".infoB")
unsigned int nonvolatilecoefficients[2] = {0x00,0x00};




void main(void)
{
  WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog timer
  if (CALBC1_1MHZ==0xFF)					// If calibration constants erased
  {
    while(1);                               // do not load, trap CPU!!
  }
  DCOCTL = 0;                               // Select lowest DCOx and MODx settings
  BCSCTL1 = CALBC1_1MHZ;                    // Set DCO to 1MHz
  DCOCTL = CALDCO_1MHZ;
  FCTL2 = FWKEY + FSSEL0 + FN1;             // MCLK/3 for Flash Timing Generator

  P1DIR = 0x31;                             // P1.0 and P1.6, P1.7 output, else input (was 0x01)
  P1OUT = nonvolatilecoefficients[0];
  P2OUT = nonvolatilecoefficients[1];


  P1OUT |=  0x0e;                            // P1.1, P1.2 and P1.3 set, else reset
  P1REN |= 0x0e;                            // P1.1, P1.2 and P1.3 pullup
  P1IE |= 0x0e;                             // P1.1, P1.2 and P1,3 interrupt enabled
  P1IES |= 0x0e;                            // P1.1, P1.2 and P1.3 Hi/lo edge
  P1IFG &= ~0x0e;                           // P1.1, P1.2 and P1.3 IFG cleared



  _BIS_SR(LPM4_bits + GIE);                 // Enter LPM4 w/interrupt
}

void variablearraytoflash (void)
{
	char *Flash_ptrB;

	Flash_ptrB = (char *) 0x1080;             // Initialize Flash segment C pointer

	FCTL1 = FWKEY + ERASE;                    // Set Erase bit
	FCTL3 = FWKEY;                            // Clear Lock bit
	*Flash_ptrB = 0;							// Dummy write to erase Flash segment B
	FCTL1 = FWKEY + WRT;                      // Set WRT bit for write operation

	nonvolatilecoefficients[0] = P1OUT;
	nonvolatilecoefficients[1] = P2OUT;


	FCTL1 = FWKEY;                            // Clear WRT bit
	FCTL3 = FWKEY + LOCK;                     // Set LOCK bit
}


// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  volatile unsigned int i;
  volatile unsigned int j;

    j = 2;                            // Nested Loop because integer
     do                                 // can't be over 64K.
     { j--;
        i = 5000;
        do
        {i--;
        }while (i !=0);
     }while (j !=0);



 // This Checks if the first switch was pressed
 if ( !(P1IN & 0x02))
  {
  P1OUT ^= 0x01;                            // P1.0 = toggle and p1.1
  variablearraytoflash();
  t++;
  }


//This checks if the second switch was pressed.
 if ( !(P1IN & 0x04))
  {
  P1OUT ^= 0x10;                            //
  variablearraytoflash();
  t--;
  }


//This checks if the third switch was pressed.
 if ( !(P1IN & 0x08))
  {
  P1OUT ^= 0x20;                            //
  variablearraytoflash();
  t--;
  }

  P1IFG &= ~0x0e;                           // clear interrupts.
}





// Port 2 Service..

// Port 1 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
  volatile unsigned int i;
  volatile unsigned int j;

    j = 2;                            // Nested Loop because integer
     do                                 // can't be over 64K.
     { j--;
        i = 5000;
        do
        {i--;
        }while (i !=0);
     }while (j !=0);



 // This Checks if the first switch was pressed
 if ( !(P2IN & 0x01))
  {
  P2OUT ^= 0x02;                            // toggle P2.1
  }

  P2IFG &= ~0x01;                           // P2.0 IFG cleared
}

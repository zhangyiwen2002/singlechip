 /*    |--------------------------MSP430F249-----------------------------|      * 
 *     | P1.0  P1.1  P2.0  P2.1  P2.2  P2.3  P4.0~P4.7  P3.5  P3.6  P3.7 |      *
 *     |  |     |     |     |     |     |       |         |     |     |  |      *
 *     | LED   LED    K1    K2    K3    K4  LCD_D0~D7    EN    RW     RS |      *
 *     | red  green                                                      |      *
 *     |-----------------------------------------------------------------|      *
 *If you want to set the time, follow the statement below:                      *
 *Press the key1 until both the red LED and green LED light to enter the setting*
 *model.                                                                        *
 *Continue press the key1 until both light off to enter the setting date model  *
 *press the key2 to set/quit set year model, key3 pre to enter/quit set month   *
 *press key1 to increase number and key4 to decrease.                           *
 *Press key4 to enter/quit set day model,press key1 to increase number and key4 *
 *to decrease.                                                                  *
 *Press the key2 until the red LED light and green LED off to enter the setting *
 *hour moudle.                                                                  *
 *In this model, press key1 let the number of hours increas and press the key4  *
 *let the number decraese.                                                      *
 *when you finish setting£¬press the key2 to quit hour setting model.           *
 *Press the key3 until the red LED off and green LED light to enter the setting *
 *minute moudle.                                                                *
 *Setting minutes number same as hours except change the key2 to key3.          *
 *When you finish setting, press the key4 to quit setting model.                *
 *                                                                              *
 *                                      Author:    Ycc Mhw                      *                                  
 *                                        date:    2 0 2 0 - 0 6 - 0 4          *
 ********************************************************************************/

#include"MSP430.h"
#include"config.h"
#include"LCD1602.h"
#include<stdio.h>
#include"clock.h"


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	                          // Stop watchdog timer
    CCR0=65535;                                           //count value
    P1DIR |= 0x03;                                        // P1.0 output
    CCTL0 = CCIE;                                         // CCR0 interrupt enabled
    TACTL = TASSEL_2 + MC_2;                              // ACLK, contmode
    P1OUT|=BIT0;                                          //let the P1.0 output 1

/*set the external interrput pin*/
    P2IE |= 0x01;                                         // P2.0 interrupt enabled
    P2IES |= 0x01;                                        // P2.0 Hi/lo edge
    P2IFG &= ~0x01;                                       // P2.0 IFG cleared

/*initial the lcd1602 and the port connect with lcd1602*/ 
    Port1602_init();
    LCD1602_init();
    LCD1602_clear();                                      
    display();
      for(;;)
    {
      __bis_SR_register(LPM0_bits + GIE);                 //enter the LPM0 open the general interrupt
    }
}


/*TimerA interrupt serve programme change the number of clock varible*/ 

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERA0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  P1OUT ^= 0x01;                                          // Toggle P1.0
  CCR0 += 50000;                                          // Add Offset to CCR0
  S=S+1;
	if(S>59)
		M=M+1,S=0;
	if(M>59)
		H=H+1,M=0;
	if(H>23)
		D=D+1,H=0;
        if(D>30)
                MON=MON+1,D=0;
        if(MON>12)
                Y=Y+1,MON=0;
        display();
}

/*interrupt serve programme change the number of clock varible*/

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(PORT1_VECTOR))) Port_2 (void)
#else
#error Compiler not supported!
#endif
{
  P1OUT = 0x03;                                           // P1.0 = toggle  
  K1=BIT0;
  K2=BIT1;
  K3=BIT2;
  K4=BIT3;
  char keyvalue;
  keyvalue=getkey();
  do
  {
       keyvalue=getkey();
       if(keyvalue==K1)
         setdate();
       if(keyvalue==K2)
         sethour();
       if(keyvalue==K3)
	 setmin();
   }while(keyvalue!=K4);
  P2IFG &= ~0x01;                                         // P1.3 IFG cleared
  P1OUT=BIT0;
}

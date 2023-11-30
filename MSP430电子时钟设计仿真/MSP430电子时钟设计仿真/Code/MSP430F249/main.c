
#include <msp430.h>
#include <stdio.h>
#include <string.h>

#define DIN_H P5OUT |= BIT0
#define DIN_L P5OUT &= ~BIT0
#define LD_H P5OUT |= BIT1
#define LD_L P5OUT &= ~BIT1
#define CLK_H P5OUT |= BIT2
#define CLK_L P5OUT &= ~BIT2

#define set_rs P1OUT |= BIT3
#define rst_rs P1OUT &= ~BIT3
#define set_rw P1OUT |= BIT4
#define rst_rw P1OUT &= ~BIT4
#define set_en P1OUT |= BIT5
#define rst_en P1OUT &= ~BIT5
#define dataout P3DIR = 0xff
#define dataport P3OUT

#define uchar unsigned char
#define CPU_F1 ((double)1000000)
#define delay_us8M(x) __delay_cycles((long)(CPU_F1*(double)x/1000000.0))
#define delay_ms8M(x) __delay_cycles((long)(CPU_F1*(double)x/1000.0))

void write_com(uchar com);
void write_data(uchar data);
void check_busy();

int display_hh =  8;
int display_mm = 30;
int display_ss = 30;
unsigned int timer_cnt = 0;

unsigned char Set_Mode = 0;

int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    P1DIR |=   BIT0 + BIT3 + BIT4 + BIT5;
    P2DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3);
    P2REN |=   BIT0 + BIT1 + BIT2 + BIT3;
    P2OUT |=   BIT0 + BIT1 + BIT2 + BIT3;

    if (CALBC1_1MHZ==0xFF)                    // If calibration constant erased
    {
    while(1);                               // do not load, trap CPU!!
    }
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;

    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    CCR0 = 46920 - 1;                         // 50ms interrupt
    TACTL = TASSEL_2 + MC_1;                  // SMCLK, upmode

    dataout;
    rst_en;
    write_com(0x38);//8 bit data, doule line, 5x7 point matrix
    write_com(0x0c);//open display, close cusor,close blink
    write_com(0x06);//auto plus 1 after write or read
    write_com(0x01);//clear scree

    write_com(0x80);
    write_data('E');
    write_data('l');
    write_data('e');
    write_data('c');
    write_data('t');
    write_data('r');
    write_data('o');
    write_data('n');
    write_data('i');
    write_data('c');
    write_data(' ');
    write_data('C');
    write_data('l');
    write_data('o');
    write_data('c');
    write_data('k');

    write_com(0xC0);
    write_data('T');
    write_data('i');
    write_data('m');
    write_data('e');
    write_data(':');
    write_data(' ');
    write_data(' ');
    write_data(display_hh/10 + '0');
    write_data(display_hh%10 + '0');
    write_data(':');
    write_data(display_mm/10 + '0');
    write_data(display_mm%10 + '0');
    write_data(':');
    write_data(display_ss/10 + '0');
    write_data(display_ss%10 + '0');

    P2IE  |=   BIT0 + BIT1 + BIT2 + BIT3;      // P2.0/1/2/3 interrupt enabled
    P2IES |=   BIT0 + BIT1 + BIT2 + BIT3;      // P2.0/1/2/3 Hi/lo edge
    P2IFG &= ~(BIT0 + BIT1 + BIT2 + BIT3);     // P2.0/1/2/3 IFG cleared

  __bis_SR_register(GIE);                   // enable interrupt
  while(1)
  {
      __delay_cycles(10000);
  }
}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    if(P2IFG & 0x01)  // P2.0 Interrupt -> Set
    {
        P2IFG &= ~BIT0;    // P2.0 IFG cleared
        Set_Mode++;
        if(Set_Mode == 4) Set_Mode = 1;
    }

    if(P2IFG & 0x02)  // P2.1 Interrupt -> OK
    {
        P2IFG &= ~BIT1;    // P2.1 IFG cleared
        Set_Mode = 0;
    }

    if(P2IFG & 0x04)  // P2.2 Interrupt -> +
    {
        P2IFG &= ~BIT2;    // P2.2 IFG cleared
        switch(Set_Mode)
        {
        case 0:
            break;
        case 1:
            display_hh++;
            break;
        case 2:
            display_mm++;
            break;
        case 3:
            display_ss++;
            break;
        default:
            break;
        }

        if(display_hh == 24) display_hh = 0;
        if(display_mm == 60) display_mm = 0;
        if(display_ss == 60) display_ss = 0;

        write_com(0xC7);
        write_data(display_hh/10 + '0');
        write_data(display_hh%10 + '0');
        write_data(':');
        write_data(display_mm/10 + '0');
        write_data(display_mm%10 + '0');
        write_data(':');
        write_data(display_ss/10 + '0');
        write_data(display_ss%10 + '0');
    }

    if(P2IFG & 0x08)  // P2.3 Interrupt -> -
    {
        P2IFG &= ~BIT3;    // P2.3 IFG cleared
        switch(Set_Mode)
        {
        case 0:
            break;
        case 1:
            display_hh--;
            break;
        case 2:
            display_mm--;
            break;
        case 3:
            display_ss--;
            break;
        default:
            break;
        }

        if(display_hh == -1) display_hh = 23;
        if(display_mm == -1) display_mm = 59;
        if(display_ss == -1) display_ss = 59;

        write_com(0xC7);
        write_data(display_hh/10 + '0');
        write_data(display_hh%10 + '0');
        write_data(':');
        write_data(display_mm/10 + '0');
        write_data(display_mm%10 + '0');
        write_data(':');
        write_data(display_ss/10 + '0');
        write_data(display_ss%10 + '0');
    }

}

// Timer A0 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERA0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  if(timer_cnt ++ == 10)  // 1s interrupt
  {
      P1OUT ^= BIT0;  // Blink LED
      timer_cnt = 0;

      if(Set_Mode == 0)  // Run Mode
      {
          display_ss ++;

          if(display_ss == 60)
          {
              display_ss = 0;
              display_mm++;
          }

          if(display_mm == 60)
          {
              display_mm = 0;
              display_hh++;
          }

          if(display_hh == 24)
          {
              display_hh = 0;
          }

          write_com(0xC7);
          write_data(display_hh/10 + '0');
          write_data(display_hh%10 + '0');
          write_data(':');
          write_data(display_mm/10 + '0');
          write_data(display_mm%10 + '0');
          write_data(':');
          write_data(display_ss/10 + '0');
          write_data(display_ss%10 + '0');
      }
  }

}


void write_com(uchar com)
{
  rst_rs;
  rst_rw;
  rst_en;
  _NOP();
  set_en;// write command rs=0,rw=0,en=1
  P3OUT = com;
  delay_ms8M(5);
  rst_en;
}

void write_data(uchar data)
{
  set_rs;
  rst_rw;
  rst_en;
  _NOP();
  set_en;//write data rs=2,rw=0,en=1
  P3OUT = data;
  delay_ms8M(5);
  rst_en;
}

//
void check_busy(){
  P3DIR &= 0x00;
  _NOP();
  _NOP();
  rst_rs;
  set_rw;
  _NOP();
  rst_en;
  _NOP();
  set_en;//read status rs=0,rw=1,en=1
  _NOP();
  _NOP();
  while((P2IN & 0x80) != 0);
  P3DIR |= 0xff;
  rst_en;
  rst_rw;
}


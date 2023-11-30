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
 *when you finish setting，press the key2 to quit hour setting model.           *
 *Press the key3 until the red LED off and green LED light to enter the setting *
 *minute moudle.                                                                *
 *Setting minutes number same as hours except change the key2 to key3.          *
 *When you finish setting, press the key4 to quit setting model.                *
 *                                                                              *
 *                                      Author:    Ycc Mhw                      *                                  
 *                                        date:    2 0 2 0 - 0 6 - 0 4          *
 ********************************************************************************/

#include"MSP430.h"
#include"LCD1602.h"
#include<stdio.h>
#include"clock.h"
#include"config.h"


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	                          // 关闭看门狗定时器
    CCR0=65535;                                           //count value 设置周期
    P1DIR |= 0x03;                                        // P1.0,1.1 设置为输出
    CCTL0 = CCIE;                                         // CCR0 开启timerA中断为比较模式中断
    TACTL = TASSEL_2 + MC_2;                              // ACLK, 增计数模式,计到CCR0
    P1OUT|=BIT0;                                          //使p1.0口输出

/*set the external interrput pin*/
    P2IE |= 0x01;                                         // P2.0 interrupt enabled中断使能为外部中断源
    P2IES |= 0x01;                                        // P2.0 Hi/lo edge 中断边缘选择为下降沿
    P2IFG &= ~0x01;                                       // P2.0 中断标志寄存器置为0
//每个PxIFGx位都是其相应I/O引脚的中断标志，并在所选输入信号边缘出现在引脚时设置。
//当相应的PxIE位和GIE位被设置时，所有PxIFGx中断标志都会请求中断。必须使用软件重置每个PxIFG标志。
//软件还可以设置每个PxIFG标志，提供一种生成软件启动中断的方法。
/*initial the lcd1602 and the port connect with lcd1602*/ 
    Port1602_init();
    LCD1602_init();
    LCD1602_clear();                                      
    display();
      for(;;)
    {
      //全局中断指的是中断的总开关，只有这个总开关打开了，其他中断事件才能够向上报。
      __bis_SR_register(LPM0_bits + GIE);                 //enter the LPM0 open the general interrupt
    }
}


/*TimerA interrupt serve programme change the number of clock varible*/ 
//设置timerA中断向量
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMERA0_VECTOR
//定义为中断函数
__interrupt void Timer_A (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMERA0_VECTOR))) Timer_A (void)
#else
#error Compiler not supported!
#endif
{
  P1OUT ^= 0x01;                                          // P1.0口取反
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
//设置p2中断向量
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

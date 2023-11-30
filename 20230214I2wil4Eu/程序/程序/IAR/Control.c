#include <msp430f249.h>
#define AHout P5OUT = 0x01;
#define ALout P5OUT = 0x02;
#define BHout P5OUT = 0x04;
#define BLout P5OUT = 0x08;
#define EndOut P5OUT = 0x00;
#define LightOut  P5OUT |= 0x10
#define Hred    BIT0
#define Hyellow BIT1
#define Hgreen  BIT2
#define Vred    BIT3
#define Vyellow BIT4
#define Vgreen  BIT5
#define Vertical 0
#define Horizontal 1
int num[17]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71,0x40};
int timeA,timeB;        //���ʱ��
int red,yellow,green;   //���̻Ƶȶ�ʱѡ��
int dir;        //����ѡ��

void SetLights(int light)       //���õ�����
{
  P6OUT = light;
  LightOut;     //����ź�
  EndOut;       //�������
}

void Init()
{
  /*��������˿�*/
  P4SEL &= 0x00;
  P4DIR |= 0xff;
  
  P5SEL &= 0x00;
  P5DIR |= 0xff;
  
  P6SEL &= 0x00;
  P6DIR |= 0xff;
  
  P1SEL &= ~0x03;
  P1DIR |= 0x03;
  P1OUT = 0x01;
  
  P4OUT = 0xff;
  P5OUT = 0xff;
  P6OUT = 0xff;

  /*��ʱ������*/
  TACTL = TASSEL_1 + MC_1; //ʱ��ѡ�� ACLK = 32768 Hz
  CCR0 = 32768;         //��ʱ 1 s 
  CCTL0 = CCIE;         //ʱ���жϴ�
  
  TBCTL = TBSSEL_1 + MC_1;
  TBCCR0 = 500;
  TBCCTL0=CCIE;
  
  /**/
  dir=Vertical;
  SetLights(Vred + Hgreen);
}

void ReSet()    //��ʱ����
{
  red=30;
  yellow=5;
  green=25;
}

void SetData()  //���д��
{
  if(dir == Horizontal)  //horizontal-red
  {
    timeA=red;
    if(green > 0)
    {
      timeB = green;
      SetLights(Hred + Vgreen);
    }
    else
    {
      timeB = yellow;
      SetLights(Hred + Vyellow);
    }
  }
  else          //vertical-red
  {
    timeB=red;
    if(green > 0)
    {
      timeA = green;
      SetLights(Vred + Hgreen);
    }
    else
    {
      timeA = yellow;
      SetLights(Vred + Hyellow);
    }
  }
}

void WriteA(int high,int low)
{
  P4OUT = high; //ʮλ
  AHout;
  EndOut;
  P4OUT = low;  //��λ
  ALout;
  EndOut;
}

void WriteB(int high,int low)
{
  P4OUT = high; //ʮλ
  BHout;
  EndOut;
  P4OUT = low;  //��λ
  BLout;
  EndOut;
}

//Timer_A��ʱ���ж�
#pragma vector=TIMERA0_VECTOR
__interrupt void Timer_A()
{
  SetData();
  WriteA(num[timeA/10],num[timeA%10]);
  WriteB(num[timeB/10],num[timeB%10]);
  
  red--;
  green--;
  if(green < 0)
  {
    yellow--;
    if(yellow <= 0)
    {
      if(dir == Horizontal) dir = Vertical;
      else dir = Horizontal;
      ReSet();
    }
  }
}

//LED�����ź�
#pragma vector=TIMERB0_VECTOR
__interrupt void Timer_B()
{
  P1OUT = ~P1OUT;
}

void main()
{
  WDTCTL = WDTPW + WDTHOLD; //�رտ��Ź�
  
  Init();       //��ʼ������
  
  ReSet();      //�������

  _BIS_SR(LPM3_bits + GIE); //��ȫ���жϣ�������͹���ģʽ
}

static int H=0,M=0,S=0,D=11,MON=3,Y=2022;     //variable of clock: Y=year, MON=month, D=day, H:hour, M=minute, S=second
unsigned char K1, K2, K3, K4;                       //setting button
unsigned char table[]={0,1,2,3,4,5,6,7,8,9};       //time number list
void getCurrentTime(void);
char getkey(void);
void setyear(void);
void setmonth(void);
void setday(void);
void sethour(void);
void setmin(void);
void display(void);
void display_year(void);

void getCurrentTime(void){
   /*获取当前时间*/
  time_t timep=time(0);
  struct tm *p;
  p=localtime(&timep);
  S=p->tm_sec;
  M=p->tm_min;
  H=8+p->tm_hour;
  D=p->tm_mday;
  MON=1+p->tm_mon;
  Y=1900+p->tm_year;
}

  //得到按了哪个键，哪一位为高电平其余为低电平
char getkey(void)
{
	char x,y;
	x=(P2IN&0x0F);// 按了哪个键哪一位为低电平
        x=(x^0x0F);//按了哪个键哪一位为高电平其余为低电平
	y=x;
	delay_ms(100);                      // key-virbation eliminate
	x=(P2IN&0x0F);
        x=(x^0x0F);     //短时间内按了两下则不算
  if(x==y)
	return x;
  else 
      return 0;
}



void setdate(void)
{
  P1OUT=0x00;
  K1=BIT0;
  K2=BIT1;
  K3=BIT2;
  K4=BIT3;
  char keyvalue;
  keyvalue=getkey();
  do
  {
       keyvalue=getkey();
       if(keyvalue==K2)
         setyear();
       if(keyvalue==K3)
         setmonth();
       if(keyvalue==K4)
	       setday();
   }
  while(keyvalue!=K1);
  P1OUT=0x03;
}

/*set the number of years*/
void setyear(void)
{
	unsigned char keyvalue;
	P1OUT=BIT0;
        keyvalue=getkey();
	do
	{ 
	keyvalue=getkey();
	 if(keyvalue==BIT0)
          {
	    Y=Y+1;    
          }
/*refresh the new setting number of year*/
         display_year();
	 if(keyvalue==BIT3)
          {
            Y=Y-1;
          }
/*refresh the new setting number of year*/
         display_year();
        }
   while(keyvalue!=BIT1);
P1OUT=0x00;
}

/*set the number of monthes*/
void setmonth(void)
{
	unsigned char keyvalue;
	P1OUT=BIT1;
        keyvalue=getkey();     //ensure the value of month in 1~12
      do
      { 
      keyvalue=getkey();
       if(keyvalue==BIT0)//按下3号键
      {
        if(MON<12)
              MON=MON+1;
        else
         M=1;          
      }
  /*refresh the new setting number of month*/
     LCD1602_write_char(8,0,table[MON/10]+0x30);//LCD第8位和第9位为月份
     LCD1602_write_char(9,0,table[MON%10]+0x30);//加48转为assic码
      if(keyvalue==BIT3)//按下4号键
       {
        if(MON>0)
               MON=MON-1;
        if(MON==0)
         MON=12;
       }
  /*refresh the new setting number of month*/
      LCD1602_write_char(8,0,table[MON/10]+0x30);//更新信息
      LCD1602_write_char(9,0,table[MON%10]+0x30);
    }
   while(keyvalue!=BIT2);
  P1OUT=0x00;
}

/*set the number of day*/
void setday(void)
{
	unsigned char keyvalue;
	P1OUT=0x03;
        keyvalue=getkey();       // ensure the value of day in 1~30
	do
	{ 
	keyvalue=getkey();
	 if(keyvalue==BIT1)//按下二号键
          {
           if(D<30)
	    D=D+1;
           else
            D=1;          
          }
/*refresh the new setting number of day*/
         LCD1602_write_char(11,0,table[D/10]+0x30);
         LCD1602_write_char(12,0,table[D%10]+0x30);
	 if(keyvalue==BIT2)//按下三号键
          {
            if(D>1)
	     D=D-1;
            else
             D=30;
          }
/*refresh the new setting number of day*/
         LCD1602_write_char(11,0,table[D/10]+0x30);
         LCD1602_write_char(12,0,table[D%10]+0x30);
        }
        while(keyvalue!=BIT3);
        P1OUT=0x00;
}

/*set the number of hours*/
void sethour(void)
{
	unsigned char keyvalue;
	P1OUT=BIT0;
        keyvalue=getkey();      // ensure the value of hour in 0~23
	do
	{ 
	keyvalue=getkey();
	 if(keyvalue==BIT0)
          {
           if(H<23)
	    H=H+1;
           else
            H=0;          
          }
/*refresh the new setting number of hour*/
         LCD1602_write_char(4,1,table[H/10]+0x30);
         LCD1602_write_char(5,1,table[H%10]+0x30);
	 if(keyvalue==BIT3)
          {
            if(H>0)
	     H=H-1;
            else
             H=23;
          }
/*refresh the new setting number of hour*/
         LCD1602_write_char(4,1,table[H/10]+0x30);
         LCD1602_write_char(5,1,table[H%10]+0x30);
        }
        while(keyvalue!=BIT1);
        P1OUT|=BIT1;
}

/*set the number of minutes*/
void setmin(void)//设置分钟
{
	unsigned char keyvalue;
	P1OUT=BIT1;//使p1.1口输出
        keyvalue=getkey();
	while(keyvalue!=BIT2)
	{
	keyvalue=getkey();
	if(keyvalue==BIT0)       // ensure the value of minute in 0~59
	{
          if(M<59)
	   M=M+1;
          else
           M=0;
        }
/*refresh the new setting number of minute*/
        LCD1602_write_char(7,1,table[M/10]+0x30);
        LCD1602_write_char(8,1,table[M%10]+0x30);
	if(keyvalue==BIT3)
	{
          if(M>0)
	   M=M-1;
          else
           M=59;
        }
/*refresh the new setting number of minute*/
        LCD1602_write_char(7,1,table[M/10]+0x30);
        LCD1602_write_char(8,1,table[M%10]+0x30);
        }
        while(keyvalue!=BIT2);
          P1OUT|=BIT0;
}


/*display the time with date in a |yyyy-mm-dd|
  format like                     | hh:mm:ss |*/

 void display(void)
{
  LCD1602_write_char(7,0,'-');
  LCD1602_write_char(10,0,'-');
/*display the year*/
  display_year();
/*display the month*/
  LCD1602_write_char(8,0,table[MON/10]+0x30);
  LCD1602_write_char(9,0,table[MON%10]+0x30);
/*display the day*/
  LCD1602_write_char(11,0,table[D/10]+0x30);
  LCD1602_write_char(12,0,table[D%10]+0x30);
  LCD1602_write_char(6,1,':');
  LCD1602_write_char(9,1,':');
/*display the hour*/
  LCD1602_write_char(4,1,table[H/10]+0x30);
  LCD1602_write_char(5,1,table[H%10]+0x30);
/*display the minute*/
  LCD1602_write_char(7,1,table[M/10]+0x30);
  LCD1602_write_char(8,1,table[M%10]+0x30);
/*display the second*/
  LCD1602_write_char(10,1,table[S/10]+0x30);
  LCD1602_write_char(11,1,table[S%10]+0x30);
}

void display_year(void)
{
  int t,h,d,s;//取出4位数字
  t=Y/1000;
  h=(Y-t*1000)/100;
  d=(Y-t*1000-h*100)/10;
  s=(Y-t*1000-h*100)%10;
  LCD1602_write_char(3,0,table[t]+0x30);
  LCD1602_write_char(4,0,table[h]+0x30);
  LCD1602_write_char(5,0,table[d]+0x30);
  LCD1602_write_char(6,0,table[s]+0x30);
}

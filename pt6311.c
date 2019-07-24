#include "pt6311.h"
#include "stdio.h"
#include <string.h>
#include <intrins.h>
//auth:katachi
//time:2017-12-30
//func:driver for pt6311
//transplant to stm8 time:2018-1-14 17:
//modify for 51 :2019年4月21日22:30:57
//auto addr sequence:
/*
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_ModeSetting|0x00);//8grid 20seg
	
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_DataSetting|0x00);	////autoAdd addr
	
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_AddressSetting|0x00);	//start addr 0x00
	WriteByte_PT6311(0x7e);											//seg0-seg7
	WriteByte_PT6311(0x18);											//seg8-seg15
	WriteByte_PT6311(0x2);											//seg16-seg23		
	
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_DisplaySetting|0x0f);//on 14/16
	STB=1;	
*/
//
//sbit DI 	=P3^5;
//sbit DO	=P1^2;
//sbit CK	=P3^4;			
//sbit STB =P3^3;	
//transplant for pt6324
//hnm18lm05t
//grid:0-3 special
//grid:4-15 米字断码
//dspseg 0-3 directly assignment
//dspseg 4-15 if number add 10 else string decrease 38


//for hnm vfd
//u8 ui_font[]={
//0x
//};

void Delay1us()		//@12.000MHz
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}
void Delay1ms()		//@12.000MHz
{
	unsigned char i, j;

	i = 12;
	j = 169;
	do
	{
		while (--j);
	} while (--i);
}
void  delay_us(u8  n){
	while(n--)Delay1us();
}
void  delay_ms(u32 n){
	while(n--)Delay1ms();
}
const u16 font[]={
	0x77,/*0*/
	0x12,/*1*/
	0x5D,/*2*/
	0x5B,/*3*/
	0x3A,/*4*/
	0x6B,/*5*/
	0x6F,/*6*/
	0x52,/*7*/
	0x7F,/*8*/
	0x7B,/*9*/
	0x21C0,/*'c*///10
	0x00C0,/*-*/
	0x2001,/*二*/
	0x20C1,/*三*/
	0x2B0f,/*四*/
	0x2AC9,/*五*/
	0x14C8,/*六*/
	0x2162,/*七*/
	0x2727,/*0*///18
	0x0224,/*1*/
	0x21C5,/*2*/
	0x22C5,/*3*/
	0x02C6,/*4*/
	0x22C3,/*5*/
	0x23C3,/*6*/
	0x0421,/*7*///0x0205,/*7*/
	0x23C7,/*8*/
	0x22C7,/*9*/
	0x08C8,/*+*///28
	0x0808,/*|*/ 
	0x1CF8,/***/
	0x0420,/*/*/
	0x1632,/*%*/	
	0x0000,//clr //33
	0x2000,/*_*/ //34
	0x06a4,/*A*/
	0x2A8D,/*B*/
	0x2103,/*C*/
	0x2A0D,/*D*/
	0x21C3,/*E*/
	0x01C3,/*F*/
	0x3383,/*G*/
	0x03C6,/*H*/
	0x2809,/*I*/
	0x1204,/*J*/
	0x1162,/*K*/
	0x2102,/*L*/
	0x0336,/*M*/
	0x1316,/*N*/
	0x2307,/*O*/
	0x01C7,/*P*/
	0x3307,/*Q*/
	0x11C7,/*R*/
	0x2291,/*S*/
	0x0809,/*T*/
	0x2306,/*U*/
	0x0522,/*V*/
	0x1706,/*W*/
	0x1430,/*X*/
	0x0830,/*Y*/
	0x2421,/*Z*/
};
/*
//ascii code :
0:48 
A:65
for small number://米
dspseg[i]=dspseg[i]-38;
for large number://8
dspseg[i]=dspseg[i];
for alphabet:
dspseg[i]=dspseg[i]-38;
*/
u8 AM=0,PM=0,HR=0,COL=0,TIMER=0,CLEAN=0,PROBE=0;
u8 dspseg[16];	//character to dis
void ReflashDisplay(void){
	u8 i;
	
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_AddressSetting|0x00);
	//transcode
	for (i=4;i<16;i++)
		if (dspseg[i]<=9)//number 0-9 to display
			dspseg[i]+=18;
		else if (dspseg[i]>47)//string to display
			dspseg[i]=dspseg[i]-30;
	//send
	for (i=0;i<16;i++){
		if (i<4){
			if ((i==1) || (i==3)){//1 3
				if (dspseg[i]<100){//larger than 100 to clr
					WriteByte_PT6311(font[dspseg[i]%10]<<3|((i==1)?(HR<<2):0));
					WriteByte_PT6311((font[dspseg[i]%10]>>5)|(font[dspseg[i]/10]<<3)|(COL<<2));
					WriteByte_PT6311(font[dspseg[i]/10]>>5);	
					}else{
						WriteByte_PT6311(0);WriteByte_PT6311(0);WriteByte_PT6311(0);
					}					
			}else{//0 2
				WriteByte_PT6311((i==2)?((PROBE<<3)|(TIMER<<1)|(CLEAN)):0);//timer
				WriteByte_PT6311((font[dspseg[i]]<<3)|PM|(AM<<1));
				WriteByte_PT6311(font[dspseg[i]]>>5);
			}
		}
		else{
			WriteByte_PT6311(font[dspseg[i]]);//low byte
			WriteByte_PT6311(font[dspseg[i]]>>8);//high byte
			WriteByte_PT6311(0);
		}
	}		
	STB=1;
}
void OpenStrobe_PT6311(void)
{
	STB=1;
	delay_us(1);
	STB=0;
}
void Init_PT6311(void)
{
	u8 i;
	
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_ModeSetting|0x0F);//16 digit 24 setment

	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_DataSetting|0x00);	////autoAdd addr
	
	OpenStrobe_PT6311();
	WriteByte_PT6311(CMD_DisplaySetting|0x8|2);//0x80:on 11/16
	
	//all on
//	OpenStrobe_PT6311();
//	WriteByte_PT6311(CMD_AddressSetting|0x00);	
//	for (i=0;i<48;i++){
//		WriteByte_PT6311(0xff);
//	}
//	delay_ms(1000);
//	//all clr
//	OpenStrobe_PT6311();
//	WriteByte_PT6311(CMD_AddressSetting|0x00);	
//	for (i=0;i<48;i++){
//		WriteByte_PT6311(0);
//	}	
//	delay_ms(1000);
//	//show number(only)
//	dspseg[0]=0;dspseg[1]=12;dspseg[2]=3;dspseg[3]=45;//8字屏 show 012345
//	for (i=4;i<16;i++)				//米字屏
//		if (i<10)dspseg[i]=i;	
//		else dspseg[i]=i-10;
//	//offset for 米字 only this way need to
//	for (i=4;i<16;i++)	dspseg[i]+=10;
//	//send to pt
//	OpenStrobe_PT6311();	
//	WriteByte_PT6311(CMD_AddressSetting|0x00);
//	for (i=0;i<16;i++){
//		if (i<4){//8字屏
//			if ((i==1) || (i==3)){//1 3
//				WriteByte_PT6311(font[dspseg[i]%10]<<3|((i==1)?(HR<<2):0));
//				WriteByte_PT6311((font[dspseg[i]%10]>>5)|(font[dspseg[i]/10]<<3)|(COL<<2));
//				WriteByte_PT6311(font[dspseg[i]/10]>>5);				
//			}else{//0 2
//				WriteByte_PT6311(0);
//				WriteByte_PT6311((font[dspseg[i]]<<3)|PM|(AM<<1));
//				WriteByte_PT6311(font[dspseg[i]]>>5);
//			}
//		}
//		else{//米字屏
//			WriteByte_PT6311(font[dspseg[i]]);//low byte
//			WriteByte_PT6311(font[dspseg[i]]>>8);//mid byte
//			WriteByte_PT6311(0);	//high byte
//		}
//	}	
//	delay_ms(1000);
	//show aplhabet(also number)
	strcpy(dspseg,"    404YETFOUNDS");
	dspseg[0]=7;dspseg[1]=77;dspseg[2]=7;dspseg[3]=77;//8字屏 show 012345
	ReflashDisplay();
	delay_ms(1000);
	for (i=0;i<16;i++)dspseg[i]=CLR;//clr
}
void WriteByte_PT6311(u8 dat)
{
	u8 i;

	CK=1;//de-pulldown
	for (i=0;i<8;i++)
	{
		CK=0;	//>>200ns
		DI=dat&0x01;		//send a bit to pt6311's data in pin
		dat>>=1;				//lsb first
		CK=1;
	}
}
//u8 ReadByte_PT6311(void)
//{
//	u8 dat,i;
//	CK=1;
//	delay_us(2);
//	for (i=0;i<8;i++)
//	{
//		CK=0;//while (j++<10);
//		delay_us(1);//tplz tpzl
//		dat>>=1;				//lsb first
//		if (DO)
//			dat|=0x80;				//catch a bit from pt6311's data out pin
//		CK=1;
//	}
//	delay_us(1);//tclk stb
//	return dat;
//}
/******************for stm8 wrong position between grid and positive********************************
unsigned int Pow2(u8 y)
{
	u16 x=1;
	if (y)
	{
		while (y--)
			x*=2;
	}
	else
		x=1;
	return x;
}
void TransCoding(void)//recongnize num or char or with colon and transcoding the dspseg to pt6311 ram
{
	u8 i=0,j=0;u16 tmp=0;
	
	for (i=0;i<30;i++)dspbuf[i]=0;//clrclr!!!!
 	for (i=0;i<13;i++)//seg==i
	 {
	   if (i==0)	//for segment 0 display temp lvl
	   {
			  tmp=Pow2(dspseg[0]) - 1;	
			  tmp<<=4;	
	   }
	   else if (i==12)//for ui
	   {
		  //dspseg[12]	8bit
		  //			_		 				_		 		_	    _		,	_ 	_ _ _
		  //  			LOWPOWER          CONTINUPAUSE 			ALL    ALARM     GPS    WEEK
		   j=dspseg[12];

		   if (j&0x08)//GPS
			   tmp|=0x80;	//=
		   if (j&0x10)//ALARM
			   tmp|=0x20;	//|=
		   if (j&0x20)//ALL
			   tmp|=0x300;
		   if (j&0x40)//CT
			   tmp|=0xc00;
		   if (j&0x80)//LP
			   tmp|=0x7000;

		   j&=0x07;//get week
		   if (j)//decrease if not 0 for clr
		   	j--;
		   if (j>4)//sat sun
			   tmp|=j+1;
		   else		//mon to fri
			   tmp|=1<<j;
	   }
	   else
	   {
		   //tmp=font['p'-87+i];//ascii to personal font
		   if (dspseg[i]>=0x80)//num with colon	!!!>=  ===
		   {
			   dspseg[i]-=0x80;
			  tmp=font[dspseg[i]]+1;
		   }
		   else if (dspseg[i]>90 && dspseg[i]<0x80)//charac
			   tmp=font[dspseg[i]-87];
		   else
				  tmp=font[dspseg[i]];//plain num
		  }
	   //transcoding
	   if(i<8)
	   {
			  for (j=0;j<15;j++)
		   {
				  dspbuf[2*j]|=(tmp&0x1)<<i;
				  tmp>>=1;	 
		   }
	  }
	   else
	   {
			  for (j=1;j<16;j++)
		   {
				  dspbuf[2*j-1]|=(tmp&0x1)<<(i-8);
				  tmp>>=1;	 
		   }		
	   }
 	}
}
void DeDisplayVfd(void)
{
  u8 len,i;

  	for (i=0;i<4;i++)
	{
	  delay_ms(500);//delay .5s
	  if (i%2)
	  {
		dspseg[0]=0;//temp
		dspseg[1]=36;//clr
		dspseg[2]=36;dspseg[3]=36;
		dspseg[4]=36;dspseg[5]=36;
		dspseg[6]=36;dspseg[7]=36;
		dspseg[8]=36;dspseg[9]=36;
		dspseg[10]=36;dspseg[11]=36;
		dspseg[12]=0;//ui
	  }
	  else
	  {
		dspseg[0]=10;//temp
		dspseg[1]='h';
		dspseg[2]='e';dspseg[3]='l';
		dspseg[4]='l';dspseg[5]='o';
		dspseg[6]=36;dspseg[7]='i';
		dspseg[8]='t';dspseg[9]='s';
		dspseg[10]='m';dspseg[11]='e';
		dspseg[12]=CONT|ALL|GPS|ALARM|LP|5;//ui
	  }
	  
	  TransCoding();
	  OpenStrobe_PT6311();
	  WriteByte_PT6311(CMD_ModeSetting|0x0e);//15digits 13sg

	  OpenStrobe_PT6311();
	  WriteByte_PT6311(CMD_DataSetting|0x04);	//fixed addr

	  for (len=0;len<30;len++)
	  {  
		OpenStrobe_PT6311();
		WriteByte_PT6311(CMD_AddressSetting|ADDR[len]);		
			WriteByte_PT6311(dspbuf[len]);
	  }
	  OpenStrobe_PT6311();
	  WriteByte_PT6311(CMD_DisplaySetting|0x08|0x04);//on 14/16	
	  STB=1;
	}
	
	//clr
  	for (i=0;i<13;i++)dspseg[i]=36;
	
  	for (i=11;i;i--)
	{
	  delay_ms(3);//delay .5s

	strcpy(dspseg+i,"helloitsme");
	  
	  TransCoding();
	  OpenStrobe_PT6311();
	  WriteByte_PT6311(CMD_ModeSetting|0x0e);//15digits 13sg

	  OpenStrobe_PT6311();
	  WriteByte_PT6311(CMD_DataSetting|0x04);	//fixed addr

	  for (len=0;len<30;len++)
	  {  
		OpenStrobe_PT6311();
		WriteByte_PT6311(CMD_AddressSetting|ADDR[len]);		
			WriteByte_PT6311(dspbuf[len]);
	  }
	  OpenStrobe_PT6311();
	  WriteByte_PT6311(CMD_DisplaySetting|0x08|0x05);//on 14/16	
	  STB=1;
	}
}
**********************************************/
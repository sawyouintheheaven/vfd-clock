#include "uart.h"
unsigned char xdata RxBuffer[64]={0};
unsigned char UART_RX_NUM=0;
#define FOSC 11059200L          //????
#define BAUD 115200             //?????
void Uart_Init(void)		//115200bps@12.000MHz	//stc15w no tim1,here using tim2
{
	    P3M0 = 0x00;
    P3M1 = 0x00;
	SCON = 0x50;                //8??????
    T2L = (65536 - (FOSC/4/BAUD));   //????????
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2?1T??, ??????2
    AUXR |= 0x01;               //?????2???1???????
  //  ES = 1;                     //????1??
  //  EA = 1;
}
void Uart_Send_Data(unsigned char dat)
{
	SBUF = dat;
	//while(!Uart_Snd_Flag);
	//Uart_Snd_Flag = 0;
	while(!TI);
	TI=0;
}

void Uart_Send_String(unsigned char *str)
{
	while(*str)
	{
		Uart_Send_Data(*str++);
	}
}
void DataDel(u8 *str,u8 len)
{
	u8 i;
	for (i=0;i<len-8;i++)
		str[i]=str[i+8];
}
void CMD_Compare(void)	//
{
  u8 tmp[9]={0};
  u8 i,len;
  
  len=UART_RX_NUM&0x3f;//get the length
 // Uart_Send_String("\r\nWhat you have in put is:\r\n");
  Uart_Send_String(RxBuffer);
  Uart_Send_String("\r\n");
  //get cmd
//  for (i=0;i<8;i++)	
//	tmp[i]=RxBuffer[i];
  strncpy(tmp,(const char*)RxBuffer,8);


  //cmd explaine
//  if (strcmp("cmd_time",(const char*)tmp)==0)
//  {
//		Uart_Send_String("now modify time!\r\n");
//		DataDel(RxBuffer,len);//del cmd to get data
//		//combine the data && transport from ascii to value
//		//ModifyTime(4,12,5,11,21,0);// 4-12 w5 tm11-21-00
//		ModifyTime((RxBuffer[0]-'0')*10 + RxBuffer[1]-'0',	//month
//								(RxBuffer[2]-'0')*10 + RxBuffer[3]-'0',//day
//								(RxBuffer[4]-'0'),											//week
//								(RxBuffer[5]-'0')*10 + RxBuffer[6]-'0',//hr
//								(RxBuffer[7]-'0')*10 + RxBuffer[8]-'0',//min
//								(RxBuffer[9]-'0')*10 + RxBuffer[10]-'0');//sec
//  }
//	else if (strcmp("cmd_dimm",(const char*)tmp)==0)
//  {
//		//Uart_Send_String("now modify brightness!\r\n");
//		DataDel(RxBuffer,len);//del cmd to get data	
//		i=(RxBuffer[0]-'0')*100+(RxBuffer[1]-'0')*10+(RxBuffer[2]-'0');
//		VFD_Brt_Set(i);
//		Uart_Send_String("dimming (0-240):");
//		Uart_Send_String(RxBuffer);Uart_Send_String("\r\n");
//  }
//	else if (strcmp("cmd_svfd",(const char*)tmp)==0)
//  {
//	//	Uart_Send_String("switch on or off vfd!\r\n");
//		DataDel(RxBuffer,len);//del cmd to get data			
//		VFD_Disp_On_Off(RxBuffer[0]-'0');
//  }
//	else if(strcmp("cmd_spwr",(const char*)tmp)==0)
//  {
//	//	Uart_Send_String("switch on or off lamp pwr!\r\n");
//		DataDel(RxBuffer,len);//del cmd to get data			
//		vpwr=RxBuffer[0]-'0';
//  }

	//' '
	for (i=0;i<len;i++)
		RxBuffer[i]=0;
	UART_RX_NUM=0;//' '
}
void Uart0_Isr()interrupt 4
{
	unsigned char res=0;
	if (RI)
	{
		RI=0;
		res=SBUF;
		if ((UART_RX_NUM&0x80)==0)//?????
		{						
			if (UART_RX_NUM&0x40)//?????????
			{
				if (res!=0x0a)UART_RX_NUM=0;//?? rst
				else UART_RX_NUM|=0x80;	//????
			}
			else
			{
				if (res==0x0d)UART_RX_NUM|=0x40;//?????
				else
				{
					RxBuffer[UART_RX_NUM&0x3f]=res;//????
					UART_RX_NUM++;
					if (UART_RX_NUM>63)UART_RX_NUM=0;
				}
			}
		}	
	}
}
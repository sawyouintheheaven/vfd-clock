#ifndef __MYIICDS3231_H
#define __MYIICDS3231_H

#include "configure.h"


//#include "sys.h"   	
//transplanted to dsd3231
//modyfied:
//1.only leave 8 bit to work
//2.change the related macro definition
//3.use newer stm8s_i2c.h
//By katachi time:2018-1-20
//new added 19-3-14:
//DS3231IIC_BufferRead() 
//new modify 19-4-12 fot stc15


////IO��������
//#define DS3231IIC_SDA_IN()  {GPIOB->CRL&=0XFFFFF0FF;GPIOB->CRL|=8<<8;}
//#define DS3231IIC_SDA_OUT() {GPIOB->CRL&=0XFFFFF0FF;GPIOB->CRL|=3<<8;}

////IO��������	 
//#define DS3231IIC_SCL    PBout(3) //SCL
//#define DS3231IIC_SDA    PBout(2) //SDA	 
//#define DS3231IIC_READ_SDA   PBin(2)  //����SDA 

//sbit DS3231IIC_SCL=P1^4;
//sbit DS3231IIC_SDA=P1^3;
//sbit DS3231RST=P1^2;			//see .c

//IIC���в�������
//void DS3231IIC_Init(void);                //��ʼ��IIC��IO��				 
void DS3231IIC_Start(void);				//����IIC��ʼ�ź�
void DS3231IIC_Stop(void);	  			//����IICֹͣ�ź�
void DS3231IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 DS3231IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 DS3231IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void DS3231IIC_Ack(void);					//IIC����ACK�ź�
void DS3231IIC_NAck(void);				//IIC������ACK�ź�

void DS3231IIC_Write_One_Byte(u8 addr,u8 dat);
u8 DS3231IIC_Read_One_Byte(u8 addr);	  
void DS3231IIC_BufferRead(u8 *buffer,u8 startAddr,u8 bufferLen);
void DS3231IIC_ReverseDirBufferRead(u8 *buffer,u8 startAddr,u8 bufferLen);

void mydelay_us(unsigned char n);
#endif

















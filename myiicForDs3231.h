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


////IO方向设置
//#define DS3231IIC_SDA_IN()  {GPIOB->CRL&=0XFFFFF0FF;GPIOB->CRL|=8<<8;}
//#define DS3231IIC_SDA_OUT() {GPIOB->CRL&=0XFFFFF0FF;GPIOB->CRL|=3<<8;}

////IO操作函数	 
//#define DS3231IIC_SCL    PBout(3) //SCL
//#define DS3231IIC_SDA    PBout(2) //SDA	 
//#define DS3231IIC_READ_SDA   PBin(2)  //输入SDA 

//sbit DS3231IIC_SCL=P1^4;
//sbit DS3231IIC_SDA=P1^3;
//sbit DS3231RST=P1^2;			//see .c

//IIC所有操作函数
//void DS3231IIC_Init(void);                //初始化IIC的IO口				 
void DS3231IIC_Start(void);				//发送IIC开始信号
void DS3231IIC_Stop(void);	  			//发送IIC停止信号
void DS3231IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 DS3231IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 DS3231IIC_Wait_Ack(void); 				//IIC等待ACK信号
void DS3231IIC_Ack(void);					//IIC发送ACK信号
void DS3231IIC_NAck(void);				//IIC不发送ACK信号

void DS3231IIC_Write_One_Byte(u8 addr,u8 dat);
u8 DS3231IIC_Read_One_Byte(u8 addr);	  
void DS3231IIC_BufferRead(u8 *buffer,u8 startAddr,u8 bufferLen);
void DS3231IIC_ReverseDirBufferRead(u8 *buffer,u8 startAddr,u8 bufferLen);

void mydelay_us(unsigned char n);
#endif

















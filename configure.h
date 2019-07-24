#ifndef _CONFIGURE_H_
#define _CONFIGURE_H_


#include <stc15.h>
//hardware io
//key p30 p31 p36
//adc p11

//ds3231 timer int p32
//VOPEN p55
//ir p33
//dht11 p10
//ds3231 iic scl=p10 sda=p37
//beep p12
//pt6324 di=14 ck=p13 stb=p15


sbit BEEP=P1^2;//low to enable
sbit IR_IO = P3^3; // IR?? ??IO
sbit CLK_INT = P3^2;
sbit VOPEN=P5^5;
sbit DATA_IO = P1^0;                //定义数据引脚
sbit DS3231IIC_SCL=P1^0;
sbit DS3231IIC_SDA=P3^7;
sbit DI =P1^4;
sbit CK	=P1^3;			
sbit STB =P1^5;	

#ifndef u8
	#define u8 unsigned char
#endif
	
#endif
#ifndef PT6311_H
#define PT6311_H


//#include "stm8s.h"
//#include "delay.h"
//#include "iostm8s103f3.h"		//inc the .h to use bit control
//new
#include "configure.h"


extern u8 dspseg[16];
extern u8 AM,PM,HR,COL,TIMER,CLEAN,PROBE;
//extern const u16 font[55];	//font

//for hnm


#define COLON 0x04	
#define CLR 33 //@font

//pin definition
//#define DI 	PC_ODR_ODR5		//@vfd board //for stm32 test//PCout(2)
//#define DO	PC_IDR_IDR6					//PCin(3)
//#define CK	PC_ODR_ODR4					//PCout(0)
//#define STB PC_ODR_ODR3					//PCout(13)
//new sbit @.c
void  delay_ms(u32 n);		
void ReflashDisplay(void);
void DeDisplayVfd(void);
void InitIo_PT6311(void);
void Init_PT6311(void);
void OpenStrobe_PT6311(void);
void WriteByte_PT6311(u8 dat);
u8 ReadByte_PT6311(void);
void TransCoding(void);//transcoding
unsigned int Pow2(u8 y);
#define CMD_ModeSetting 0x00
#define CMD_DataSetting 0x40
#define CMD_AddressSetting 0xc0
#define CMD_DisplaySetting 0x80

#endif

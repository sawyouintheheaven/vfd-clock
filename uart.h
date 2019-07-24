#ifndef _UART_H_
#define _UART_H_
#include "stc15.h"
#include "string.h"

extern unsigned char xdata RxBuffer[64];
extern unsigned char UART_RX_NUM;
void Uart_Init(void)	;
void Uart_Send_Data(unsigned char dat);
void Uart_Send_String(unsigned char *str);

void DataDel(unsigned char *str,unsigned char len);
void CMD_Compare(void);

#endif
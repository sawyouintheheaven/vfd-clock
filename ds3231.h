#ifndef DS3231_H
#define DS3231_H
#include "myiicForDs3231.h"

#define DS3231_WriteAddress 0xD0    //����д��ַ 
#define DS3231_ReadAddress  0xD1    //��������ַ

#define DS3231_SECOND       0x00    //��
#define DS3231_MINUTE       0x01    //��
#define DS3231_HOUR         0x02    //ʱ
#define DS3231_WEEK         0x03    //����
#define DS3231_DAY          0x04    //��
#define DS3231_MONTH        0x05    //��
#define DS3231_YEAR         0x06    //��
//����1            
#define DS3231_SALARM1ECOND 0x07    //��
#define DS3231_ALARM1MINUTE 0x08    //��
#define DS3231_ALARM1HOUR   0x09    //ʱ
#define DS3231_ALARM1WEEK   0x0A    //����/��
//����2
#define DS3231_ALARM2MINUTE 0x0b    //��
#define DS3231_ALARM2HOUR   0x0c    //ʱ
#define DS3231_ALARM2WEEK   0x0d    //����/��
#define DS3231_CONTROL      0x0e    //���ƼĴ���
#define DS3231_STATUS       0x0f    //״̬�Ĵ���
#define BSY                 2       //æ
#define OSF                 7       //����ֹͣ��־
#define DS3231_XTAL         0x10    //�����ϻ��Ĵ���
#define DS3231_TEMPERATUREH 0x11    //�¶ȼĴ������ֽ�(8λ)
#define DS3231_TEMPERATUREL 0x12    //�¶ȼĴ������ֽ�(��2λ)  

u8 BCD2DEC(u8 val);    //BCDת��ΪByte
u8 DEC2BCD(u8 val);    //B��ת��ΪBCD��

void ModifyTime(u8 year,u8 mon,u8 da,u8 wk,u8 hou,u8 min,u8 sec);
void SetAlarm(u8 hou,u8 min,u8 sec,u8 en);
void clrAlarm(void);
u8 get_year(void);
void get_show(unsigned char * normalTime);
void get_time(unsigned char * normalTime);
void get_date(unsigned char * normalTime);
void get_show_Temperature(unsigned char  *tenTimesTemp);
void WeekDisplay(u8 w);
#endif


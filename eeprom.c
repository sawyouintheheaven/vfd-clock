#include "eeprom.h"
#include "intrins.h"
#include "stc15.h"


/*----------------------------
�ر�IAP
----------------------------*/
void IapIdle()
{
    IAP_CONTR = 0;                  //�ر�IAP����
    IAP_CMD = 0;                    //�������Ĵ���
    IAP_TRIG = 0;                   //��������Ĵ���
    IAP_ADDRH = 0x80;               //����ַ���õ���IAP����
    IAP_ADDRL = 0;
}

/*----------------------------
��ISP/IAP/EEPROM�����ȡһ�ֽ�
----------------------------*/
BYTE IapReadByte(WORD addr)
{
    BYTE dat;                       //���ݻ�����

    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_READ;             //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    dat = IAP_DATA;                 //��ISP/IAP/EEPROM����
    IapIdle();                      //�ر�IAP����

    return dat;                     //����
}

/*----------------------------
дһ�ֽ����ݵ�ISP/IAP/EEPROM����
----------------------------*/
void IapProgramByte(WORD addr, BYTE dat)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_PROGRAM;          //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_DATA = dat;                 //дISP/IAP/EEPROM����
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}

/*----------------------------
��������
----------------------------*/
void IapEraseSector(WORD addr)
{
    IAP_CONTR = ENABLE_IAP;         //ʹ��IAP
    IAP_CMD = CMD_ERASE;            //����IAP����
    IAP_ADDRL = addr;               //����IAP�͵�ַ
    IAP_ADDRH = addr >> 8;          //����IAP�ߵ�ַ
    IAP_TRIG = 0x5a;                //д��������(0x5a)
    IAP_TRIG = 0xa5;                //д��������(0xa5)
    _nop_();                        //�ȴ�ISP/IAP/EEPROM�������
    IapIdle();
}
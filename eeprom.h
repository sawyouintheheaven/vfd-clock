#ifndef _EEPROM_H_
#define _EEPROM_H_


#define CMD_IDLE 0
#define CMD_READ 1
#define CMD_PROGRAM 2
#define CMD_ERASE 3
#define MileageAddr 64
#define BLaddr 1024
#define BYTE unsigned char
#define WORD unsigned int
#define ENABLE_IAP  0x82   
void IapIdle();
BYTE IapReadByte(WORD addr);
void IapProgramByte(WORD addr, BYTE dat);
void IapEraseSector(WORD addr);

#endif
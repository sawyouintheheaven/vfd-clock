#include "myiicForDs3231.h"
#include <intrins.h>
#include "ds3231.h"			//only for iic addr




void mydelay_us(unsigned char n){
	unsigned char i;
	for (i=0;i<n;i++){
		_nop_();
		_nop_();
		_nop_();
		_nop_();	
	}
}
//void Delay50ms()		//@12.000MHz
//{
//	unsigned char i, j, k;

//	_nop_();
//	_nop_();
//	i = 3;
//	j = 72;
//	k = 161;
//	do
//	{
//		do
//		{
//			while (--k);
//		} while (--j);
//	} while (--i);
//}
//��ʼ��IIC
//void DS3231IIC_Init(void)
//{					     
////	GPIO_InitTypeDef GPIO_InitStructure;
////	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE );	
////	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //remapping pb4,3 from jtag to general gpio
////	
////	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
////	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
////	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
////	GPIO_Init(GPIOB, &GPIO_InitStructure);
//// 
////	DS3231IIC_SCL=1;
////	DS3231IIC_SDA=1;
////	GPIO_ResetBits(GPIOB,GPIO_Pin_4);
////	delay_ms(100);
////	GPIO_SetBits(GPIOB,GPIO_Pin_4);
//	
//	DS3231IIC_SCL=1;
//	DS3231IIC_SDA=1;
//	DS3231RST=0;
//	Delay50ms();
//	DS3231RST=1;
//}
//����IIC��ʼ�ź�
void DS3231IIC_Start(void)
{
	//DS3231IIC_SDA_OUT();     //sda�����
	DS3231IIC_SDA=1;	  	  
	DS3231IIC_SCL=1;
	mydelay_us(4);
 	DS3231IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	mydelay_us(4);
	DS3231IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void DS3231IIC_Stop(void)
{
	//DS3231IIC_SDA_OUT();//sda�����
	DS3231IIC_SCL=0;
	DS3231IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
 	mydelay_us(4);
	DS3231IIC_SCL=1; 
	DS3231IIC_SDA=1;//����I2C���߽����ź�
	mydelay_us(4);							   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 DS3231IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	//DS3231IIC_SDA_IN();      //SDA����Ϊ����  
	DS3231IIC_SDA=1;mydelay_us(1);	   
	DS3231IIC_SCL=1;mydelay_us(1);	 
	while(DS3231IIC_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			DS3231IIC_Stop();
			return 1;
		}
	}
	DS3231IIC_SCL=0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void DS3231IIC_Ack(void)
{
	DS3231IIC_SCL=0;
	//DS3231IIC_SDA_OUT();
	DS3231IIC_SDA=0;
	mydelay_us(2);
	DS3231IIC_SCL=1;
	mydelay_us(2);
	DS3231IIC_SCL=0;
}
//������ACKӦ��		    
void DS3231IIC_NAck(void)
{
	DS3231IIC_SCL=0;
	//DS3231IIC_SDA_OUT();
	DS3231IIC_SDA=1;
	mydelay_us(2);
	DS3231IIC_SCL=1;
	mydelay_us(2);
	DS3231IIC_SCL=0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void DS3231IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	//DS3231IIC_SDA_OUT(); 	    
    DS3231IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        DS3231IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
		mydelay_us(2);   //��TEA5767��������ʱ���Ǳ����
		DS3231IIC_SCL=1;
		mydelay_us(2); 
		DS3231IIC_SCL=0;	
		mydelay_us(2);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 DS3231IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	//DS3231IIC_SDA_IN();//SDA����Ϊ����
    for(i=0;i<8;i++ )
	{
        DS3231IIC_SCL=0; 
        mydelay_us(2);
		DS3231IIC_SCL=1;
        receive<<=1;
        if(DS3231IIC_SDA)receive++;   
		mydelay_us(1); 
    }					 
    if (!ack)
        DS3231IIC_NAck();//����nACK
    else
        DS3231IIC_Ack(); //����ACK   
    return receive;
}
void DS3231IIC_Write_One_Byte(u8 addr,u8 dat){
	DS3231IIC_Start();
	DS3231IIC_Send_Byte(DS3231_WriteAddress);
	DS3231IIC_Wait_Ack();
	DS3231IIC_Send_Byte(addr);
	DS3231IIC_Wait_Ack();
	DS3231IIC_Send_Byte(dat);
	DS3231IIC_Wait_Ack();
	DS3231IIC_Stop();	
}
u8 DS3231IIC_Read_One_Byte(u8 addr){
		u8 tmp;

	DS3231IIC_Start();
	DS3231IIC_Send_Byte(DS3231_WriteAddress);
	DS3231IIC_Wait_Ack();
	DS3231IIC_Send_Byte(addr);
	DS3231IIC_Wait_Ack();
	
	DS3231IIC_Start();
	DS3231IIC_Send_Byte(DS3231_ReadAddress);
	DS3231IIC_Wait_Ack();
	tmp = DS3231IIC_Read_Byte(0);
	DS3231IIC_Stop();	
  
	return tmp;
}
void DS3231IIC_BufferRead(u8 *buffer, u8 startAddr, u8 bufferLen)
{  	
	u8 t,add=startAddr;
	
	for(t=0;t<bufferLen;t++)
	{
			*buffer=DS3231IIC_Read_One_Byte(add++);
			buffer++;
	}
}
void DS3231IIC_ReverseDirBufferRead(u8 *buffer, u8 startAddr, u8 bufferLen)
{  	
	u8 t,add=startAddr;
	
	for(t=0;t<bufferLen;t++)
	{
			*buffer=DS3231IIC_Read_One_Byte(add--);
			buffer++;
	}
}
























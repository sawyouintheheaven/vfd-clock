#include "ds3231.h"

u8 BCD2DEC(u8 val)    //BCD转换为DEC
{
    u8 temp;
    temp=(val>>4)*10+(val&0x0f);
   
    return temp;
}

u8 DEC2BCD(u8 val)    //DEC码转换为BCD码
{
    u8 k;
    k=(val%10)+((val/10)<<4);

    return k;
}
void clrAlarm(void)
{
	DS3231IIC_Write_One_Byte(DS3231_STATUS,0);
}
void SetAlarm(u8 hou,u8 min,u8 sec,u8 en){
	u8 temp=0;
	
		DS3231IIC_Write_One_Byte(DS3231_CONTROL,0x04|en);//enable alarm 1
		DS3231IIC_Write_One_Byte(DS3231_ALARM1WEEK,0x80);//match hr min sec
	
    temp=DEC2BCD(hou);
    DS3231IIC_Write_One_Byte(DS3231_ALARM1HOUR,temp);   //修改时
   
    temp=DEC2BCD(min);
    DS3231IIC_Write_One_Byte(DS3231_ALARM1MINUTE,temp); //修改分
   
    temp=DEC2BCD(sec);
    DS3231IIC_Write_One_Byte(DS3231_SALARM1ECOND,temp); //修改秒
	
		clrAlarm();
}
void ModifyTime(u8 year,u8 mon,u8 da,u8 wk,u8 hou,u8 min,u8 sec)
{
    u8 temp=0;
	
    temp=DEC2BCD(year);
    DS3231IIC_Write_One_Byte(DS3231_YEAR,temp);  //修改year
	
    temp=DEC2BCD(mon);
    DS3231IIC_Write_One_Byte(DS3231_MONTH,temp);  //修改月
   
    temp=DEC2BCD(da);
    DS3231IIC_Write_One_Byte(DS3231_DAY,temp);    //修改日
	
    temp=DEC2BCD(wk);
    DS3231IIC_Write_One_Byte(DS3231_WEEK,temp);   //修改年 //week   
	
    temp=DEC2BCD(hou);
    DS3231IIC_Write_One_Byte(DS3231_HOUR,temp);   //修改时
   
    temp=DEC2BCD(min);
    DS3231IIC_Write_One_Byte(DS3231_MINUTE,temp); //修改分
   
    temp=DEC2BCD(sec);
    DS3231IIC_Write_One_Byte(DS3231_SECOND,temp); //修改秒
	
}
void get_show(unsigned char * normalTime)//5->0 M d w h m s
{
  u8 dat[6],i; //store time
	
	DS3231IIC_ReverseDirBufferRead(dat,DS3231_MONTH,6);//5->0 M d w h m s
	//I2C_EE_BufferRead(dat,4,3);//d m y-->0 1 2
	//dat[0]=0x12;dat[1]=0x13;dat[2]=0x22;dat[3]=0x12;dat[4]=0x13;dat[5]=0x22;
	
	dat[2]&=0x3f;//get true hour
	//bcd to dec
	for (i=0;i<6;i++)	
	{
		//dat[i]=BCD2DEC(dat[i]);
			normalTime[i]=BCD2DEC(dat[i]);
	}
		
}
u8 get_year(void){
	u8 y=BCD2DEC(DS3231IIC_Read_One_Byte(DS3231_YEAR));
	return y;
}
void get_date(unsigned char * normalTime)
{
  u8 dat[3],i; //store time
	
	DS3231IIC_ReverseDirBufferRead(dat,DS3231_MONTH,3);//5->0 M d w


	//bcd to dec
	for (i=0;i<3;i++)	//only 3 byte, do not use hardware to get week
	{
			normalTime[i]=BCD2DEC(dat[i]);
	}	
}
void get_time(unsigned char * normalTime)
{
  u8 dat[3],i; //store time
	
	DS3231IIC_ReverseDirBufferRead(dat,DS3231_HOUR,3);//h m s
	//I2C_EE_BufferRead(dat,4,3);//d m y-->0 1 2
	//dat[0]=0x12;dat[1]=0x13;dat[2]=0x22;dat[3]=0x12;dat[4]=0x13;dat[5]=0x22;
	//bcd to dec
	for (i=0;i<3;i++)	//only 3 byte, do not use hardware to get week
	{
			normalTime[i]=BCD2DEC(dat[i]);
	}		
}

void get_show_Temperature(unsigned char  *tenTimesTemp)//show temp and date
{
    u8 temp[2]; 
  //temph  _(sign) _ _ _, _ _ _ _
	//templ (point)_ _0 0, 0 0 0 0 
    DS3231IIC_BufferRead(temp,DS3231_TEMPERATUREH,2);  
   
    temp[0]=BCD2DEC(temp[0]);//int,default in positive temperature
		*tenTimesTemp=temp[0];
		//temp[1]=(temp[1]>>6)*25;//decimal 
		//tenTimesTemp=temp[0]+temp[1];
		//tenTimesTemp*=10;
}



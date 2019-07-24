
//https://github.com/sawyouintheheaven/vfd-clock

#include "configure.h"
#include <intrins.h>
#include <string.h>
#include "pt6311.h"
#include "ds3231.h"
#include "dht11.h"
#include "eeprom.h"
#include "uart.h"
//this is old board

//rom mapper
//addr data
//0 up or down
//1 on hr
//2 off hr
//3 alr hr
//4 alr min
//5 alr sec
//6 alr enable

//var
unsigned char trg,cont;//for key san
//flag
bit key_flag,flash_flag,tmp_flag,posSecOrTmp;//pos 1 up 0 down
//IR
bit Irprot_LastState = 0,Ir_finished=0;
u8 codeCnt = 0;					//0x1f - - - 1,1 1 1 1 to record decode time
u8 irTime;
u8 IR_data[4]; 
//clk
//alarm
u8 alarmMin;
bit alarm1_flag,alarm1_enable;
u8 a1_hr,a1_min,a1_sec;
// set
u8 year,mth,day,wk,hour,min,sec;//modify time
u8 state=14;//state machine
//display
u8 dis_hr,dis_min,dis_sec;
u8 dimming=3,mode=1,on_hr,off_hr,off_flag=0;
//recounter
u8 cnt_ctl,ctl_flag=0;
		//0-7    //0:auto 1:hand
//ModifyTime(4,12,5,11,21,0);// 4-12 w5 tm11-21-00
//0 set year //added for 2-29
//1 set month
//2 set day
//3 set week
//4 set hour
//5 set min
//6 set sec
//7 set on hr
//8 set off hour
//9 set sec and tmp pos
//10 set alarm hr
//11 set alr min
//12 set alr sec
//13 set enable alarm
//14 default time week temperature  date display
//func
void t0_init(void);
void keyscan(void);
void adc_init();//ch2
unsigned int getADCresult(u8 ch);//ch1
void adc_dimming(void);
void alarm_int0_init(void);

void normalDisplay(void);

void stateMachineInput(void);
void stateMachineOutput(void);
void ringing(u8 t);
//main
int main(void)
{
	u8 tmp;

	t0_init();
	adc_init();
	//ModifyTime(6,25,2,13,8,0);
	//SetAlarm(21,0,0);
	alarm_int0_init();
	VOPEN=0;//on
	PROBE=!mode;
	Init_PT6311();//initialize pt6311 
//	IapEraseSector(0);//erase before write
//	IapProgramByte(0,1);
//	IapProgramByte(1,12);
//	IapProgramByte(2,23);
	posSecOrTmp=IapReadByte(0);
	on_hr=IapReadByte(1);
	off_hr=IapReadByte(2);
	a1_hr=IapReadByte(3);
	a1_min=IapReadByte(4);
	a1_sec=IapReadByte(5);
	TIMER=alarm1_enable=IapReadByte(6);
	SetAlarm(a1_hr,a1_min,a1_sec,alarm1_enable);//write
	while (1)
	{
		if(Ir_finished)        //解码完成后，执行键值的功能
		{
			Ir_finished=0;
			if (IR_data[2]){//cap???
					ringing(100);
					alarm1_flag=0;
					VOPEN=0;	//clr alarm		//then ringing
					ctl_flag=1;//pressed	
					cnt_ctl=0;//clr
			}
			//display
			dspseg[4]='I';dspseg[5]='R';dspseg[6]=CLR;
			tmp=IR_data[2]>>4;//bcd
			if (tmp>9)tmp+=7;//display 10 with a
			dspseg[7]=tmp+10;
			tmp=IR_data[2]&0x0f;//bcd
			if (tmp>9)tmp+=7;//display 10 with a
			dspseg[8]=tmp + 10;
			//cmd to key
			switch (IR_data[2]){
				case 0xe2://close pwr
					break;
				case 0x90://set
					trg=(trg&0x01)|0x01;
					break;
				case 0xe0://+
					trg=(trg&0x02)|0x02;
					break;
				case 0xa8://-
					trg=(trg&0x40)|0x40;
					break;
				case 0x30://
					mode=1;
					dimming=0;
					break;
				case 0x18://
					dimming=1;mode=1;
					break;
				case 0x7a://
					dimming=2;mode=1;
					break;
				case 0x10://
					dimming=3;mode=1;
					break;
				case 0x38://
					dimming=4;mode=1;
					break;
				default:break;
			}
			stateMachineInput();
		}
		if (key_flag){
				key_flag=0;
				keyscan();
				if (trg&0x1 || trg&02 || trg&0x40){//pressed ?
					ringing(250);
					alarm1_flag=0;
					VOPEN=0;	//clr alarm		//then ringing
					ctl_flag=1;//pressed	
					cnt_ctl=0;//clr
				}			
				stateMachineInput();					
		}
		if (flash_flag){
			flash_flag=0;
			//state machine
			stateMachineOutput();
//			adc_dimming(); //do not frequent move to@if (tmp_flag){//5s
      //reflash
			ReflashDisplay();
     } 
	}
}
void Alarm_Int0() interrupt 0
{
	u8 dat[3];
	get_time(dat);
	alarm1_flag=1;	//闹铃标志位置位
	alarmMin=dat[1]; //初始化闹铃时间
	clrAlarm();//clr
}
void t0_isr()interrupt 1
{//256us == .25ms //125us
	static u8 cnt_key,cnt_flash,cnt_tmp;
	
	//flag
	if (++cnt_key>=100){//12.5ms
		cnt_key=0;
		key_flag=1;
		if (++cnt_flash>=20){//500ms
			cnt_flash=0;
			flash_flag=1;
			if (++cnt_tmp>=10){//5s
				cnt_tmp=0;
				tmp_flag=1;
				if (ctl_flag){
						if (++cnt_ctl>=4){//20s reverse counter 
							cnt_ctl=0;ctl_flag=0;
							state=14;
						}
				}
			}
		}
	}
	//ir
	irTime++;
	if(irTime==240) {							//240*125us = 30ms
		irTime--; //hold 
		codeCnt=0x3f;//finish state	rst 1,1 1 1 1 == 0x1f
	} 
	if(IR_IO) Irprot_LastState=1; //last time io state
	else if(Irprot_LastState) {		//negative edge
		Irprot_LastState = 0;				//clr
		if(irTime<24) {							//24*125us=3ms negedge data bit
			codeCnt++; codeCnt &= 0x1f;	//decode time ++
			IR_data[codeCnt>>3] <<= 1;	//1,1 1 1 1 == 0x1f, >> 3 == 1 1 (2^3=8 bit) save IR_data index 
		if( irTime>15 ) 						//15*125us=1.875ms bit 1
			IR_data[codeCnt>>3]++;		//bit 1
		if (codeCnt==31)						
			Ir_finished=1;
		}
		irTime = 0; 
	}
}
void stateMachineOutput(void){
      switch (state){//every .5s update state machine 
				case 0://state 0
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='Y';dspseg[9]='E';dspseg[10]='A';dspseg[11]='R';
					dspseg[12]=2;dspseg[13]=0;dspseg[14]=year/10;dspseg[15]=year%10;					
					break;
				case 1:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='M';dspseg[9]='T';dspseg[10]='H';dspseg[11]=dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=mth/10;dspseg[15]=mth%10;
					break;
				case 2:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='D';dspseg[9]='A';dspseg[10]='Y';dspseg[11]=dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=day/10;dspseg[15]=day%10;     
					break;
				case 3:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='W';dspseg[9]='E';dspseg[10]='E';dspseg[11]='K';dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=wk+10;dspseg[15]=wk;
					break;
				case 4:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='H';dspseg[9]='O';dspseg[10]='U';dspseg[11]='R';dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=hour/10;dspseg[15]=hour%10;
					break;
				case 5:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='M';dspseg[9]='I';dspseg[10]='N';dspseg[11]=dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=min/10;dspseg[15]=min%10;
					break;
				case 6:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='S';dspseg[9]='E';dspseg[10]='C';dspseg[11]=dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=sec/10;dspseg[15]=sec%10;
					break;
				case 7:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='O';dspseg[9]='N';dspseg[10]=CLR;dspseg[11]='H';dspseg[12]='R';dspseg[13]=CLR;					
					dspseg[14]=on_hr/10;dspseg[15]=on_hr%10;
					break;
				case 8:
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;
					dspseg[8]='O';dspseg[9]='F';dspseg[10]='F';dspseg[11]='H';dspseg[12]='R';dspseg[13]=CLR;
					dspseg[14]=off_hr/10;dspseg[15]=off_hr%10;
					break;
				case 9://set pos
					dspseg[4]='S';dspseg[5]='E';dspseg[6]='T';dspseg[7]=CLR;dspseg[8]='P';dspseg[9]='O';dspseg[10]='S';dspseg[11]=CLR;
					if (posSecOrTmp){
						dspseg[12]='U';dspseg[13]='P';dspseg[14]=dspseg[15]=CLR;
					}
					else{
						dspseg[12]='D';dspseg[13]='O';dspseg[14]='W';dspseg[15]='N';
					}
					break;
				case 10://alr hour
					dspseg[4]='A';dspseg[5]='L';dspseg[6]='M';dspseg[7]=CLR;
					dspseg[8]='H';dspseg[9]='O';dspseg[10]='U';dspseg[11]='R';dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=a1_hr/10;dspseg[15]=a1_hr%10;
					break;
				case 11:
					dspseg[4]='A';dspseg[5]='L';dspseg[6]='M';dspseg[7]=CLR;
					dspseg[8]='M';dspseg[9]='I';dspseg[10]='N';dspseg[11]=dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=a1_min/10;dspseg[15]=a1_min%10;
					break;
				case 12:
					dspseg[4]='A';dspseg[5]='L';dspseg[6]='M';dspseg[7]=CLR;
					dspseg[8]='S';dspseg[9]='E';dspseg[10]='C';dspseg[11]=dspseg[12]=dspseg[13]=CLR;
					dspseg[14]=a1_sec/10;dspseg[15]=a1_sec%10;
					break;
				case 13:
					dspseg[4]='A';dspseg[5]='L';dspseg[6]='M';dspseg[7]=CLR;dspseg[8]='S';dspseg[9]='W';dspseg[10]=dspseg[11]=CLR;
					if (alarm1_enable){
						dspseg[12]='O';dspseg[13]='N';dspseg[14]=dspseg[15]=CLR;
					}
					else{
						dspseg[12]='O';dspseg[13]='F';dspseg[14]='F';dspseg[15]=CLR;
					}
					break;
				case 14:
					normalDisplay();
				break;
				default:break;
      }
}
void stateMachineInput(void){    
		u8 dat[6];static u8 hold;
		if (state){
				get_show(dat);//5->0 M d w h m s
				min=dat[4];sec=dat[5];//get current time to modify	
		}  		
		switch (state){
				case 0://set year
        if (trg&0x01){//set
            state=1;
          }
				if (trg&0x40)//short press 
					if (++year>99) year=0;
				if (trg&0x02)//short press 
					if (--year >99) year=99;
				if (cont&0x40)//long press
					if (++hold>=22){
						hold=0;
						if (++year>99) year=0;
				}
				if (cont&0x02)//long press
					if (++hold>=22){
						hold=0;
						if (--year >99) year=99;
				}					
				break;
        case 1://set mth
        if (trg&0x01){//set
            state=2;
          }
		if (trg&0x40)//short press 
			if (++mth>12) mth=1;
          if (trg&0x02)//short press 
            if (--mth <1) mth=12;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++mth>12) mth=1;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--mth <1) mth=12;
		}
          break;
        case 2://set day
        if (trg&0x01){//set
            state=3;
          }
          if (trg&0x40)//+
            if (++day>31) day=1;
          if (trg&0x02)//-
            if (--day >31) day=31;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++day>31) day=1;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--day >31) day=31;
		}
          break;
        case 3://set wk
        if (trg&0x01){//set
            state=4;
          }
          if (trg&0x40)//+
            if (++wk>7) wk=1;
          if (trg&0x02)//-
            if (--wk <1) wk=7;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++wk>7) wk=1;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--wk <1) wk=7;
		}
          break;
        case 4://set hr
        if (trg&0x01){//set
            state=5;
          }
          if (trg&0x40)//+
            if (++hour>23) hour=0;
          if (trg&0x02)//-
            if (--hour>23) hour=23;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++hour>23) hour=0;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--hour>23) hour=23;
		}
          break;
        case 5://set min
        if (trg&0x01){//set
            state=6;
          }
          if (trg&0x40){//+
            if (++min>59) min=0;
			ModifyTime(year,mth,day,wk,hour,min,sec);
			}
          if (trg&0x02){//-
            if (--min >=59) min=59;
			ModifyTime(year,mth,day,wk,hour,min,sec);
			}
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
            if (++min>59) min=0;
			ModifyTime(year,mth,day,wk,hour,min,sec);
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
            if (--min >=59) min=59;
			ModifyTime(year,mth,day,wk,hour,min,sec);
		}
          break;
        case 6://set sec
        if (trg&0x01){//set
            state=7;//turn back
            ModifyTime(year,mth,day,wk,hour,min,sec);
          }
          if (trg&0x40){//+
            if (++sec>59) sec=0;
						ModifyTime(year,mth,day,wk,hour,min,sec);
					}
          if (trg&0x02){//-
            if (--sec>59) sec=59;
						ModifyTime(year,mth,day,wk,hour,min,sec);
					}
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
            if (++sec>59) sec=0;
						ModifyTime(year,mth,day,wk,hour,min,sec);
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
            if (--sec>59) sec=59;
						ModifyTime(year,mth,day,wk,hour,min,sec);
		}
          break;
        case 7://set on hr
        if (trg&0x01){//set
            state=8;//turn back
			IapEraseSector(0);   //erase before write
			IapProgramByte(1,on_hr);
          }
          if (trg&0x40)//+
            if (++on_hr>23) on_hr=0;
          if (trg&0x02)//-
            if (--on_hr>23) on_hr=23;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++on_hr>23) on_hr=0;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--on_hr>23) on_hr=23;
		}
          break;
        case 8://set off hr
        if (trg&0x01){//set
            state=9;//turn back
			IapProgramByte(2,off_hr);
          }
          if (trg&0x40)//+
            if (++off_hr>23) off_hr=0;
          if (trg&0x02)//-
            if (--off_hr>23) off_hr=23;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++off_hr>23) off_hr=0;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--off_hr>23) off_hr=23;
		}
          break;     
		case 9://sec pos
        if (trg&0x01){//set
            state=10;//turn back
			IapProgramByte(0,posSecOrTmp);
          }
          if (trg&0x40)//+
            posSecOrTmp=!posSecOrTmp;
          if (trg&0x02)//-
            posSecOrTmp=!posSecOrTmp;						
		break;	
        case 10://set al hr
        if (trg&0x01){//set
            state=11;
			IapProgramByte(3,a1_hr);
          }
          if (trg&0x40)//+
            if (++a1_hr>23) a1_hr=0;
          if (trg&0x02)//-
            if (--a1_hr>23) a1_hr=23;
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
			if (++a1_hr>23) a1_hr=0;
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
			if (--a1_hr>23) a1_hr=23;
		}
         break;
        case 11://set a1 min
        if (trg&0x01){//set
            state=12;
			IapProgramByte(4,a1_min);
          }
          if (trg&0x40){//+
            if (++a1_min>59) a1_min=0;
			
			}
          if (trg&0x02){//-
            if (--a1_min >=59) a1_min=59;
			
			}
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
            if (++a1_min>59) a1_min=0;
			
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
			hold=0;
            if (--a1_min >=59) a1_min=59;
			
		}
          break;
        case 12://set a1 sec
        if (trg&0x01){//set
            state=13;//turn back
            IapProgramByte(5,a1_sec);
          }
          if (trg&0x40){//+
            if (++a1_sec>59) a1_sec=0;
						
					}
          if (trg&0x02){//-
            if (--a1_sec>59) a1_sec=59;
						
					}
	  if (cont&0x40)//long press
		if (++hold>=22){
			hold=0;
            if (++a1_sec>59) a1_sec=0;
						
		}
	  if (cont&0x02)//long press
		if (++hold>=22){
            if (--a1_sec>59) a1_sec=59;
						
		}
          break;	
		case 13://enable alr 1
        if (trg&0x01){//set
            state=14;//turn back
			IapProgramByte(6,alarm1_enable);
			SetAlarm(a1_hr,a1_min,a1_sec,alarm1_enable);//write
          }
          if (trg&0x40)//+
            alarm1_enable=!alarm1_enable;
          if (trg&0x02)//-
            alarm1_enable=!alarm1_enable;		
					TIMER=alarm1_enable;
		break;		
        case 14://set or change dimming
        if (trg&0x01){//set
            state=0;//set year
						get_show(dat);//5->0 M d w h m s
						mth=dat[0];day=dat[1];wk=dat[2];hour=dat[3];min=dat[4];sec=dat[5];//get current time to modify
						year=get_year();					
          }
         if (trg&0x40){//+
            dimming++;
            if (dimming<4)
              mode=1;//hand
            else if (dimming==4)
              mode=0;//auto
            else if (dimming==5)
              dimming=3,mode=1;
						
            if (!mode)
							PROBE=1;
						else
							PROBE=0;
						
            dspseg[0]=dimming;
            VOPEN=0;//wake up
            adc_dimming();
         }
         if (trg&0x02){//-
            if (--dimming>7)
              dimming=0,VOPEN=1; 
            dspseg[0]=dimming;
            adc_dimming();
         }
          break;					
        default:break;
        }
}
void ringing(u8 t){
	u8 i;
	for ( i=0;i<t;i++){
		BEEP=0;
		mydelay_us(150);	
		BEEP=1;
		mydelay_us(150);
	}
}
void adc_dimming(void){
				u8 tmp;
	//adc
	if (!mode){
			tmp=getADCresult(1);//ch1 8bit
//			dspseg[4]=tmp/100 +10;
//			dspseg[5]=tmp%100/10+10;
//			dspseg[6]=tmp%10+10;	
			dimming=tmp/64;//256/4=64
	}
			OpenStrobe_PT6311();
			WriteByte_PT6311(CMD_DisplaySetting|0x8|dimming);//0x80:on 11/16	
}
void normalDisplay(void){//display and alarm
	u8 dat[5];static u8 last;
			if (tmp_flag){//5s
				tmp_flag=0;
				//humity and temp
				EA=0;//clr int
				GetData(dat);//interval more than 1s
				Check(dat);
				EA=1;
			if (!posSecOrTmp){//down
				dspseg[3]=dat[2];//tmp
			}			
			else{//up
				dspseg[10]=dat[2]/10;//tmp
				dspseg[11]=dat[2]%10;//tmp
			}
				
				dspseg[13]=dat[0]/10;//humity
				dspseg[14]=dat[0]%10;//humity
				dspseg[15]=32;//'%'
				
				//date
				get_date(dat);
				dspseg[4]=dat[0]/10;		//mth
				dspseg[5]=dat[0]%10;			
				dspseg[6]=dat[1]/10;		//day
				dspseg[7]=dat[1]%10;
				//dspseg[8]=21;//'-' addres:21
				//wk
				dspseg[8]=dat[2]+10;	//offset to chinese ui	
				//adc auto dimming
				adc_dimming();			
			}
			//time
			get_time(dat);
			dis_hr=dat[0];
			dis_min=dat[1];
			dis_sec=dat[2];
			
			dspseg[0]=dis_hr/10;		//hr/10
			dspseg[1]=(dis_hr%10)*10+dat[1]/10;		//hr%10 + min/10
			dspseg[2]=dis_min%10;	  //min%10
			dspseg[9]=CLR;//clr
			if (!posSecOrTmp){//down
				dspseg[10]=dis_sec/10;			//sec
				dspseg[11]=dis_sec%10;		
				dspseg[12]=CLR;
			}			
			else{//up
				dspseg[3]=dis_sec;			//sec
				dspseg[12]=10;//''c'				
			}
			//ui
			if (last!=dis_sec)				//for col blink
				last=dis_sec,COL=!COL;
			if (dis_hr>12)
				PM=1,AM=0;
			else
				PM=0,AM=1;
			//alarm
			if (alarm1_flag){//alarm??
				if (dis_min!=alarmMin)
					BEEP=1,alarm1_flag=0;
				else{
					ringing(200);
					ringing(200);
				}
			}		
			//on off vopen         
			if (dis_hr==off_hr)//match sw off hour 
			  if (!off_flag){	
					off_flag=1;		//set off flag
					VOPEN=1;		//clr
					mode=1;//hand
			  }
			if (dis_hr==on_hr)//match sw on hour
				VOPEN=0,off_flag=0,mode=0;//open pwr auto dimming clr
}
void keyscan(void)
{
     unsigned char tmp=P3^0xff;
     trg=tmp&(tmp^cont);
     cont=tmp;
}
void alarm_int0_init(){
	EA = 1;
	EX0 = 1;
	IT0 = 0;//电平的触发方式
}
void adc_init()//ch1
{
	P1ASF=0x02;
	ADC_CONTR = 0X89;
	ADC_RES=0;
}
unsigned int getADCresult(u8 ch)
{
	unsigned int buf=0;
	ADC_CONTR = 0x88 | ch;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	while (!(ADC_CONTR&0X10));
	//buf = (ADC_RES<<2)|ADC_RESL; //10bit
	buf = ADC_RES;
	return buf;
}
void t0_init(void){//125us
	AUXR &= 0x7F;		//?????12T??
	TMOD &= 0xF0;		//???????
	TMOD |= 0x02;		//???????
	TL0 = -125;		//??????
	TH0 = -125;		//???????
	TF0 = 0;		//??TF0??
	TR0 = 1;		//???0????
	ET0 = 1;
	EA = 1;
}

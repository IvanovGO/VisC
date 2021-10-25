#include <iom88.h>
#include <inavr.h>
#include <string.h>
#include "viscmtr2.h"
#define CLK 11
//Changed resolution to 0.0625C (DS18B20)
#define _DIR DDRD_Bit5
#define _PORT PORTD_Bit5
#define _PIN PIND_Bit5
#define ADCwait()
/*static void ADCwait(void){
   ADCflag=0;__enable_interrupt();
   while(!ADCflag);__disable_interrupt(); }*/
static void TXbyte(unsigned char byte){
   unsigned char t;
	for(t=0;t<8;t++){ ADCwait();
		if(byte&1){
		   _PORT=0;_DIR=1;   /*DQ low*/
			__delay_cycles(5*CLK); //5 us
			_DIR=0;_PORT=1;	/*and immediately release*/
			__delay_cycles(90*CLK);   }	//if
		else {
			_PORT=0;_DIR=1;   /*DQ low*/
			__delay_cycles(90*CLK); //90 us
			_DIR=0;_PORT=1;	/*release*/
			__delay_cycles(5*CLK);	}	//else
		byte>>=1;	}
	}
static int RXbyte(void){
   unsigned char byte,t;
	for(byte=0,t=0;t<8;t++){ ADCwait();
		byte>>=1;
		_PORT=0;_DIR=1;	//DQ low
      __delay_cycles(5*CLK); //5 us
		_DIR=0;_PORT=1;		//DQ high(released)
      __delay_cycles(5*CLK); //5 us
		byte|=128*_PIN;	//sample
		__delay_cycles(90*CLK);   }	//for
	return byte;	}

static unsigned char scratchpad[9];
extern __flash unsigned char crc8tab[];
__monitor void ds18x20read(void){   //called every 30ms.
   static char _1820cnt=0,conversion=0;char t,crc8;
   if(++_1820cnt<40)return;   //at least 750 ms period (18B20 tconv).
   _1820cnt=0;
   //RESET and PRESENCE pulses
   _PORT=0;_DIR=1;   //RESET
   __delay_cycles(720*CLK);   //720 us
   _DIR=0;_PORT=1;   //pullup
   __delay_cycles(70*CLK);   //70 us
   for(t=0;t<20;t++){
      if(!_PIN)goto presence;
      __delay_cycles(10*CLK);}   //10 us
   data.temp=-10101;conversion=0;
   return;//No presence pulse during 240us after rising RESET edge
presence:   //something has responded
   ADCwait();  //one ADC int.
   __delay_cycles(960*CLK);  //960 us
   if(!_PIN){data.temp=-10102;conversion=0;return;}//constant zero. Something has gone wrong.
   ADCwait();if(conversion)goto conv;  //[start] or [read] selection
   //Issue Skip Rom, then Convert Temp commands
   TXbyte(0xCC);TXbyte(0x44);_PORT=1;_DIR=1; //Strong pullup
   //Wait at least 500 ms
   conversion=1;return;
conv: //1s after strong pullup activation,RESET and PRESENCE check
   conversion=0;
   //Skip Rom,Read Scratchpad
   TXbyte(0xCC);TXbyte(0xBE);
   for(t=0;t<9;t++)scratchpad[t]=RXbyte();
   for(t=0,crc8=0;t<9;t++)crc8=crc8tab[crc8^scratchpad[t]];
   if(crc8){data.temp=-10103;return;}//CRC8 check failed,data incorrect
   data.temp=*(signed int*)scratchpad*(scratchpad[4]&128?8:1); //DS18S20 or DS18B20
}  //DS18x20 read

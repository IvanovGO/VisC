#include <iom48.h>
#include <inavr.h>
#include <string.h>
#include "viscmtr2.h"
//11.0592MHz crystal oscillator
//Added sinc filter with 32-samples window
//Pin assignments:
/* PD0/RXD - RS232 rxd
   PD1/TXD - RS232 txd
   PD2 - NC
   PD3/OC2B - PWM output for LM317
   PD4 - NC
   PD5 - DQ for DS1820
   PD6 - LED 1
   PD7 - LED 2
   PB0 - LED 3
   PB1,2 - NC
   PB3/MOSI - NC
   PB4/MISO - A0,channel select
   PB5/SCK - A1,channel select
   PB6,7 - crystal
   PC0 - NC
   PC1/ADC1 - MCP601 out
   PC2,3 - NC
   PC4/ADC4 - motor current
   PC5 - NC   */
#define XON 17
#define XOFF 19
#define set_channel(x) PORTB=(PORTC&0xCF)|(x<<4)
#define LED1 PORTD_Bit6
#define LED2 PORTD_Bit7
#define LED3 PORTB_Bit0
#define ADMUX_VT 1*64+1 //AVCC,ADC1
#define ADMUX_IM 3*64+4 //internal VREF,ADC4
__regvar __no_init unsigned int acc1@12,acc2@14,acc@10;
__regvar __no_init char tx_en@9;
union tagPacket data;
#define VTSLEN 128   //temperature buffer length
#define IMSLEN 32    //motor current buffer length
unsigned short vts[VTSLEN],ims[IMSLEN];
unsigned long vta,ima;  //accumulators
unsigned char ivts=0,iims=0;  //indexes
void txbuf(void){
   char t;
   data.crc=data.vt+data.im+data.temp;
   for(t=0;t<sizeof(union tagPacket);t++){
      while(!UCSR0A_Bit5);UDR0=data.raw[t];  }
   }
#define rdADC() {ADCSRA_Bit6=1;while(ADCSRA_Bit6);}
void read_vt(void){  //approx. 30 ms
   char t;unsigned short ts;
   acc1=0,acc2=0,acc=0;
   ADMUX=ADMUX_VT;
   set_channel(1);   //pos.
   rdADC();ADC;
   for(t=0;t<64;t++){rdADC();acc1+=ADC;}
   set_channel(0);   //charge
   ADMUX=ADMUX_IM;rdADC();ADC;
   for(t=0;t<32;t++){rdADC();acc+=ADC;}
   ADMUX=ADMUX_VT;
   set_channel(3);   //neg.
   rdADC();ADC;
   for(t=0;t<64;t++){rdADC();acc2+=ADC;}
   set_channel(0);   //charge
   ADMUX=ADMUX_IM;rdADC();ADC;
   for(t=0;t<32;t++){rdADC();acc+=ADC;}
//   sinc filter
   ts=(acc2>acc1)?(acc2-acc1):(acc1-acc2);
   vta-=vts[ivts];ima-=ims[iims];   //subtract i-th sample
   vts[ivts]=ts;ims[iims]=acc;   //store new sample
   vta+=ts;ima+=acc; //add new sample
   if(++ivts>=VTSLEN)ivts=0;  //advance in circular buffer
   if(++iims>=IMSLEN)iims=0;  //advance in circular buffer
   data.vt=vta/VTSLEN;
   data.im=ima/IMSLEN;
   LED1=(data.temp>-100); //DS18x20
   LED2=(data.im>1200);   //motor
   }
__monitor void ds18x20read(void);   //from 1wire.c
void init(void){
   DDRD=2+8+64+128;DDRC=0;DDRB=1+16+32;
   PORTD=0xFF;PORTC=1+4+8+32;PORTB=2+4+8;
   ACSR=128;   //disable comparator
   //UART init
   UBRR0=5;    //115200 baud
   UCSR0A=64;   //TXC clear
   UCSR0B=16+8; //RXEN,TXEN
   UCSR0C=3*2;  //8 bit,no parity,1 stop
   //watchdog init
   MCUSR=0;__watchdog_reset();
//   WDTCSR=(1<<WDCE)+(1<<WDE);//WDTCSR=(1<<WDIE)+(1<<WDE)+3;
   WDTCSR=16+8;WDTCSR=8+5;__watchdog_reset();   //reset in 0.5 s.
   //ADC init
   ADMUX=ADMUX_VT;
   ADCSRA=128+7;  //f=CK/128=86.4kHz (6.6ksps)
   ADCSRB=0;
   //Timer2 init
   TCCR2A=2*16+3; //Fast PWM on OC2B,TOP=0xFF
   TCCR2B=1;   //f=CK
   OCR2B=0;  //242 for 4.75V
}  //init
//#define tx_en LED3   //PC transmit
__C_task void main(void){
   char d;
   init();
   tx_en=0;
//   __enable_interrupt();
loop:
   if(UCSR0A_Bit7)if( (d=UDR0) == 255)tx_en=0;
                  else OCR2B=d,tx_en=1;
   read_vt();
   ds18x20read();
   __watchdog_reset();
   LED3=tx_en;
   if(tx_en)txbuf();
   goto loop;
}

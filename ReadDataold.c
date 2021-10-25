#include <termios.h>
#include "ViC.h"
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

int OpenConnection(char *PortName[])
{

int fid;
int i;

  struct termios tio;
  
fid = open(*PortName, O_RDWR  ); // | O_NOCTTY
if (fid <0) {
	perror("Ошибка открытия порта\n");
	}	 else  {if ( DEBUG_MODE) {printf("Порт успешно открыт-%i\n",fid);}	

  if(tcgetattr(fid, &tio) < 0) {perror("Ошибка чтения аттрибутов"); } else
		 if ( DEBUG_MODE) {
    printf("input mode flags c_iflag-%i\n",tio.c_iflag);		/* input mode flags */
    printf("output mode flags c_oflag-%i\n",tio.c_oflag);		/* output mode flags */
    printf("control mode flags c_cflag-%i\n",tio.c_cflag);		/* control mode flags */
    printf("local mode flags c_lflag-%i\n",tio.c_lflag);		/* local mode flags */
    printf("line discipline c_line-%i\n",tio.c_line);			/* line discipline */
printf("c_cc");

for ( i=0;i<NCCS+1;i++) {
   printf(" [%i]=%i",i,tio.c_cc[i]);		/* control characters */}
   printf("\n");
    printf("input speed c_ispeed-%lu\n",tio.c_ispeed);		/* input speed */
    printf("output speed c_ospeed-%lu\n",tio.c_ospeed);		/* output speed */
}


    if(cfsetispeed(&tio, B115200) < 0) //Устанавливаем скорость обмена

    {
        perror("Неправильное значение скорости");
        return -1;
    } else {if ( DEBUG_MODE) {printf("Скорость успешно установлена\n"); }   }

// Делаем так, как делает uses Serial в паскале (все обнуляем)
tio.c_iflag=0;
tio.c_oflag=0;
tio.c_cflag=6322;
tio.c_lflag=0;
tio.c_line=0;
for (i=0;i<NCCS+1;i++) { tio.c_cc[i]=0;}	

    if(tcsetattr(fid,  TCSANOW,  &tio) < 0) // Применяем установки
    {
        perror("Ошибка установки аттрибутов порта");
        return -1;
    } else {if ( DEBUG_MODE) {printf("Аттрибуты порта успешно применены. Скорость %i\n",tio.c_ospeed);}}}
    	return fid;
}





int GetData(int fid, int data, char *databuff, int count)
/* 	fd - файловый дескриптор открытого порта
     	data - число для записи (величина PWM)
     	databuf - ссылка на буфер, куда будут записываться данные
     	count - число раз, сколько нужно получить данные
*/
{


if (write(fid, &data, 1)<0) 	{perror("write error\n");
							return (-1);}
				 else if (DEBUG_MODE) {printf("Значение %i успешно отправлено в порт\n",data);}
sleep(1);

struct read_buffer  rb; //Буфер чтения
count=1000;
int i;
for (i=0;i<count;i++)
{
     if (read(fid, &rb, 8)< 0){ perror("read error\n");} else 
 {   while ( ( (rb.vt==rb.im)  ||  (rb.temp==rb.crc) ) &&  (rb.im+rb.vt+rb.temp==rb.crc)  )
    { printf("lol");
    read(fid, &rb, 8);}}

if ( 1 )/**/
{printf("No-%i, vt-%i, im-%i, temp-%i, crc-%i\n",i,rb.vt,rb.im,rb.temp,rb.crc);} else {printf("Ошибка CRC\n");}

}
}


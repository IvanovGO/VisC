#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
 #include <math.h>
 #include <time.h>
 
#include "ReadData.h"
//#define DEBUG_MODE 0
#define DEBUG_MODE 1

FILE *fw;
unsigned short sdata;
long int ttime;


double V_mv(unsigned short vt){
double V;
V=vt;
V=(V-26)/2650;
return V;}

double t_C1(double u){double C1;C1=0.289*u*u+56.763*u+28.559;return C1;} //W-Re

double t_C2(double u){ double C2; C2=0.229*u*u+58.037*u+15.652;return C2;} //W-Re

double  t_S(double u){
double S;
S=0.08264*u*u*u-3.30838*u*u+126.53718*u+18.29401; return S;} //Pt

double t_K(double u){
double K;
K=0.00166*u*u*u-0.10407*u*u+25.75714*u-2.47424;return K;}//K-type (Cr-Al)


/*struct  TRead_buffer {
      unsigned short vt;
      unsigned short im;
      signed short temp;
      unsigned short crc;
      };*/

struct TRead_buffer averaging(struct TRead_buffer *p_buf,int count){
/*printf("VT=");for (int i=0;i<300;i++) printf("%i-",p_buf[i].vt);printf("\n");
printf("IM=");for (int i=0;i<300;i++) printf("%i-",p_buf[i].im);printf("\n");
printf("TEMP=");for (int i=0;i<300;i++) printf("%i-",p_buf[i].temp);printf("\n");
*/
unsigned long long svt=0;
unsigned long long sim=0;
signed long long st=0;
float stf;

for (int i=0;i<300;i++) { svt+=p_buf[i].vt;
						 sim+=p_buf[i].im;
					         st+=p_buf[i].temp; }
svt/=300; sim/=300; st/=300; 


//printf("svt=%li; sim=%li; st=%f;\n", svt,sim,st/16 );

unsigned long long dsvt=0;
unsigned long long dsim=0;
signed long long dst=0;


for (int i=0;i<300;i++) { dsvt+=p_buf[i].vt*p_buf[i].vt-svt*svt;
						 dsim+=p_buf[i].im*p_buf[i].im-sim*sim;
					         dst+=p_buf[i].temp*p_buf[i].temp-st*st; }
dsvt/=300; dsim/=300; dst/=300; 
dsvt=sqrt(dsvt); dsim=sqrt(dsim); dst=sqrt(dst); 
//printf("dsvt=%li; dsim=%li; dst=%li;\n", dsvt,dsim,dst );

for (int i=0;i<300;i++)  if (((abs(p_buf[i].vt-svt)>3*dsvt))||(abs(p_buf[i].im-sim)>3*dsim)||(abs(p_buf[i].temp-st)>3*dst)) {
p_buf[i].vt=svt;p_buf[i].im=sim;p_buf[i].temp=st;}

 svt=0;
sim=0;
st=0;

for (int i=0;i<300;i++) { svt+=p_buf[i].vt;
						 sim+=p_buf[i].im;
					         st+=p_buf[i].temp; }
svt/=300; sim/=300; st/=300; 
stf=st;
stf/=16;
//double stS=t_S(V_mv((double)svt));
double stS=t_K(V_mv((double)svt))+stf;
ttime=time(NULL);

printf("time=%li svt=%li;t=%4.1f; PWM=%li; sim=%li; stf=%2.1f;\n",ttime, svt,stS,sdata,sim,stf );
fprintf(fw,"%li;%4.1f;%li;%li;%2.1f;\n",ttime, stS,sdata,sim,stf );

}



int main (int argc, char *argv[])

{
#include "intro.c" //хвастовство

int fd,i;


struct TRead_buffer rb;

fd=POpen(argv[1]);

fw=fopen(argv[3],"a+");
if (fw==NULL) return -1;

sdata=atoi(argv[2]);






/*struct  TRead_buffer {
      unsigned short vt;
      unsigned short im;
      signed short temp;
      unsigned short crc;
      };*/
      

struct TRead_buffer stat_buf[300];

//printf("%i\n",sizeof(stat_buf));

unsigned short *u_temp;


while(1){

PWrite(fd,sdata);
//printf("\n");
for (i=0;i<300;i++) { stat_buf[i]=PRead(fd);
u_temp=stat_buf[i].temp;
//printf("%i %i %i %i\n",stat_buf[i].vt,stat_buf[i].im,u_temp,stat_buf[i].crc);
   if ((stat_buf[i].vt+stat_buf[i].im+stat_buf[i].temp)!=stat_buf[i].crc) {
i--;
//printf("%i ",i);
}}

rb=averaging(stat_buf,i);

//while ((rb.vt+rb.im+rb.temp)==rb.crc);
//printf("%i\n",rb.temp);

//printf("i=;%lu;t=;%lu;v=;%lu\n",sumi,sumt,sumv);
 
//printf("%i\n",rb.im);}

//printf("i=;%lu;v=;%lu\n");
//fprintf(fw,"i=;%lu;v=;%lu\n");
fflush(fw);}

/*while (1){rb=PRead(fd);
}*/

close(fd);
fclose(fw);
}

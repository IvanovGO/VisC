#include <stdio.h>

int StrToInt(char *s)
{
  int temp = 0; // число
  int i = 0;
  int sign = 0; // знак числа 0- положительное, 1 — отрицательное
  if (s[i] == '-')
  {
    sign = 1;
    i++;
  }
  while (s[i] >= 0x30 && s[i] <= 0x39)
  {
    temp = temp + (s[i] & 0x0F);
    temp = temp * 10;
    i++;
  }
  temp = temp / 10;
  if (sign == 1)
    temp = -temp;
  return(temp);
}


int main(int argc, char *argv[], char *env[] )

 {
 
int rez = 102;


printf("Второй параметр командной строки-%s\n",argv[1]);
 rez =   StrToInt(argv[1]);
 
 if (argc==2){
 printf("Введено значение %i\n",rez);} else {puts("Ничего не введено");}
}

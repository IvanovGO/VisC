#include <stdio.h>


void main(){

char ch;
printf("Enter char-");
scanf("%c",&ch);
printf("Entered-%c\n",ch);

switch (ch){
case 'a': puts("a");break;
case 'b':puts("b");break;
case 'c':puts("c");break;
default:puts("I dont know");
}

}

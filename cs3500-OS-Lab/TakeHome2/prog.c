#include<stdio.h>
#include<stdlib.h>
int main(){
 int * data;
data = (int*)malloc(100*(sizeof(int)));
data[100]=0;

return 0;
}

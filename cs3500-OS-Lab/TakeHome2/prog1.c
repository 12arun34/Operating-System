#include<stdio.h>
#include<stdlib.h>
int main(){
 int * data;
data = (int*)malloc(100*(sizeof(int)));
free(data);
printf("%d\n",data[1]);

return 0;
}

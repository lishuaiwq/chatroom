#include"client.h"

int main(int argc,char *argv[])
{
  if(argc!=2)
  {
     printf("输入参数错误\n");
	 exit(1); 
  } 
  Client s(atoi(argv[1]));
   s.Start(); 
   return 0;
} 

#include"server.h"
int main(int argc,char *argv[])
{
   if(argc!=2)
   {
      cout<<"参数出错"<<endl;
	  exit(1); 
   } 
   Server s(atoi(argv[1]));
//   printf("port=%d",atoi(argv[1])); 
   s.start();
   return 0;
} 

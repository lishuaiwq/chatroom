#ifndef _SERVER_H__
#define _SERVER_H__
#include"common.h"
#endif
//创建服务器端的类
class Server
{
public:
		   Server(int _port);//构造函数初始化端口号
		   ~Server();//析构函数
           void init();//初始化客户端吧
		   void start();//启动服务
		   void Close();//关闭
           int epolladd(int& fd,bool epollet);
private:
		 int port;  
		 int listenfd;//监听套接字
		 int epfd;//epoll_create返回的套接字
         list<int> client_list;//客户端列表
		 struct sockaddr_in server;//需要绑定服务器端的套接字
		 int sendbroadcastmsg(int clientfd);//广播给客户端 
}; 

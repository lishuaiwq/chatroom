#include"server.h"

Server::Server(int _port):port(_port)//构造函数初始化端口号
{
   server.sin_family=AF_INET;
   server.sin_port=htons(port);
   server.sin_addr.s_addr=htonl(INADDR_ANY);
   epfd=0;
   listenfd=0;
} 
Server::~Server()//析构函数
{} 		
int Server::epolladd(int& fd,bool epollet)
{
	struct epoll_event ev;//创建epoll_ctl要使用的结构体
	printf("fd=%d",fd); 
	ev.events=EPOLLIN;//监听读时间
    if(epollet==true)
	{
	  ev.events=(EPOLLIN | EPOLLET);//epoll边缘触发i
	  printf("\n.......\n"); 
	} 
	int fg=fcntl(fd,F_GETFL);
	if(fg<0)
	{
	   perror("use fg");
	   exit(1); 
	}  
	int ff=fcntl(fd,F_SETFL,fg | O_NONBLOCK);//设置epoll非阻塞模式
	if(ff<0)
	{
	   perror("use ff");
	   exit(1); 
	}  
	ev.data.fd=fd;
	int ep=epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev);
	if(ep<0)
	{
	   perror("use epoll_ctl");
	   exit(1); 
	} 
	cout<<"add fd to epoll success"<<endl; 
} 
void Server::init()//初始化服务器吧
{
	 cout<<"init server..."<<endl; 	
     listenfd=socket(AF_INET,SOCK_STREAM,0); 
	 if(listenfd<0)
	 {
	    perror("use socket");
		exit(1); 
	 } 
	 int Bin=bind(listenfd,(struct sockaddr*)&server,sizeof(server));
	 if(Bin<0)
	 {
	    printf("bind is error") ;
		exit(1); 
	 } 
     epfd=epoll_create(EPOLL_SIZE);//创建epoll时间
	 if(epfd<0)
	 {
	    perror("use epoll");
		exit(1); 
	 } 
	 listen(listenfd,5); 
	 epolladd(listenfd,true);//将socket套接字添加到epoll当中 
} 
void Server::start()//启动服务
{
  struct epoll_event events[10];//创建epoll_wait需要接收的数组 
  init(); 
  while(1)
  {
             //首先来判断epoll时间
			 int epollcount=epoll_wait(epfd,events,10,-1);
			 if(epollcount<0)
			 {
			   perror("use epoll");
			   break; 
			 }else
			 {
			   cout<<"epoll_wait返回成功"<<endl;
			 } 
			 //接下来处理epoll时间
			 for(int i=0;i<epollcount;i++)
			 { 
                     int sockfd=events[i].data.fd;//依次拿出epoll中的文件描述符
                     printf("sockfd=%d\n",sockfd);
					 printf("listen=%d\n",listenfd); 
					 if(sockfd==listenfd)//表示有客户端发起连接了
					 {
					    struct sockaddr_in client;
						socklen_t client_length=sizeof(struct sockaddr_in);
			  int clientfd=accept(listenfd,(struct sockaddr*)&client,&client_length);
			  if(clientfd<0)
			  {
			     perror("use accept");
				 exit(1); 
			  } 
			     cout<<"client connect from:"\
						 <<inet_ntoa(client.sin_addr)<<":"\
						 <<ntohs(client.sin_port)<<endl; 

                        epolladd(clientfd,true);//加入epoll
                         
						//用vector保存客户端信息
                       client_list.push_back(clientfd);//将客户端fd加入进去
				  cout << "Add new clientfd = "<< clientfd << "to epoll" << endl;
				  cout<<"现在有:"<<client_list.size()<<"个成员在线"<<endl;
                  //服务器给客户端发送欢迎消息
				 // cout<<"欢迎来到聊天室"<<endl;
                  char message[BUF_SIZE];
                  bzero(message,BUF_SIZE);
				  sprintf(message,"欢迎来到聊天室:%d",clientfd);
				  int ret=send(clientfd,message,BUF_SIZE,0);
				  if(ret<0)
				  {
				     perror("use send");
		             exit(-1); 
				  } 
				}
				else//处理用户发来的消息并且广播给所有的用户
				{
					 int ret=sendbroadcastmsg(sockfd);//走到这里证明有用户发来消息了
                     {
					    if(ret<0)
						{
						  perror("use sendbroadcastmsg");
						  exit(1); 
						} 
					 } 
			    } 
			 } 
         } 
} 
int Server::sendbroadcastmsg(int clientfd)//广播给客户端
{
    char buf[BUF_SIZE],message[BUF_SIZE];
	bzero(buf,BUF_SIZE);
	bzero(message,BUF_SIZE); 
	cout<<"read from client ( clientID = "<<clientfd<< ")"<<endl;

    int len=recv(clientfd,buf,BUF_SIZE,0);//为0和read作用一样
      
    if(len==0)
	{
	  close(clientfd);
      client_list.remove(clientfd);//删除客户信息
	  cout<<"client:"<<clientfd<<"quit"<<endl;
      cout<<"现在有"<<client_list.size()<<"个人在聊天室中"<<endl; 
	} 
	else//广播发给所有用户
	{
		char arr[]="当前聊天室只有你一个人"; 	
	   if(client_list.size()==1)//如果聊天室只有一个客户端
	   {
	      send(clientfd,arr,strlen(arr),0);
		  return len;
	   } 
	  sprintf(message, "client id %d say >> %s ", clientfd, buf);
	  list<int>::iterator it=client_list.begin();
	  for(;it!=client_list.end();++it) //遍历用户信息
	  {
	      if(*it!=clientfd)
		  {
		     if(send(*it,message,BUF_SIZE,0)<0)
					 return -1;
		  } 
	  } 
	} 
	return len;
} 

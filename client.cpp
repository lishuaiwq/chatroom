#include"client.h"

Client::Client(int _port):port(_port) 
{   
		Server.sin_family=AF_INET;
		Server.sin_port=htons(port);
		inet_pton(AF_INET,"0.0.0.0",(void *)&Server.sin_addr.s_addr);
		sock=0;
		pid=0;
		isClientwork=true;
		epfd=0;
} 
    // 连接服务器
void Client::Connect()
{
   cout<<"Connect Server:"<<Server.sin_addr.s_addr<<":"<<port<<endl;
   sock=socket(AF_INET,SOCK_STREAM,0);
   if(sock<0)
   {
      perror("sock error");
	  exit(-1); 
   } 
   if(connect(sock,(struct sockaddr*)&Server,sizeof(Server))<0)
   {
       perror("connect error");
	   exit(-1); 
   } 
   if(pipe(pipe_fd)<0)//创建管道用于父子间进程通信,初始化管道
   {
       perror("pipe error");
	   exit(-1);
   }
   epfd=epoll_create(EPOLL_SIZE);//创建句柄
   if(epfd<0)
   {
      perror("epfd error"); 
      exit(-1); 
   } 
   addfd(sock,true);
   addfd(pipe_fd[0],true);//将读事件添加进去 
} 
void Client::addfd(int& fd,bool isET)
{
   struct epoll_event ev;
   if(isET==true)//设置ET模式
   {
        fcntl(fd,F_SETFL,fcntl(fd,F_GETFL,0) | O_NONBLOCK);//设置非阻塞
        ev.events=EPOLLIN | EPOLLET;//设置的监听事件
		ev.data.fd=fd;//监听的文件描述符
   } 
   epoll_ctl(epfd,EPOLL_CTL_ADD,fd,&ev); 
    cout<<"fd add to epoll"<<endl;
} 
    // 断开连接
void Client::Close()
{
  if(pid)
  {
     close(pipe_fd[0]); 
  } else
  {
     close(pipe_fd[1]); 
  } 
} 
    // 启动客户端
void Client::Start()
{
   struct epoll_event events[2];//就绪事件的集合
   Connect();//发起连接
   pid=fork();
   if(pid<0)
   {
      perror("fork error");
	  close(sock);
	  exit(-1);
   } else if(pid==0)
   {
   //子进程负责写数据
   close(pipe_fd[0]);  
   cout<<"Please input 'exit' to exit the char room "<<endl;  
   while(isClientwork)//客户端工作正常
   {
      bzero(&message,BUF_SIZE);
	  fgets(message,BUF_SIZE,stdin); 
      if(strncasecmp(message,"exit",strlen("exit"))==0)
	  {
	     isClientwork=false;
	  } else
	  {
	    if(write(pipe_fd[1],message,strlen(message)-1)<0)
		{
		   perror("write error");
		   exit(-1); 
		} 
	  } 
    }
   }else：//父进程
   {
     close(pipe_fd[1]);  
   while(isClientwork)
   {//epoll不仅可以监听socket还可以监听文件描述符
     int epollcount=epoll_wait(epfd,events,2,-1);
	 for(int i=0;i<epollcount;++i)
	 {
	    bzero(&message,BUF_SIZE); 
	    if(events[i].data.fd==sock)
		{
	int ret=recv(sock,message,BUF_SIZE,0);//读取服务器发来的数据   
	      if(ret==0)//表示服务器关闭
		  {
		     cout<<"Server closed connect"<<sock<<endl;
			 close(sock);
			 isClientwork=false;
		  }else
		  {
		     cout<<message<<endl;
		  } 
		}else//否则就是子进程有数据进来了
		{
	     int ret=read(events[i].data.fd,message,BUF_SIZE); 	
		  if(ret==0)
		  {
		     isClientwork=0;
		  }else
		  {
		     send(sock,message,BUF_SIZE,0); 
		  } 
		} 
	 } 
   } 
  } 
   Close(); 
} 


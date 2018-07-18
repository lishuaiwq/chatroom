#ifndef _CLIENT_H__
#define _CLIENT_H__
#include"common.h"

#endif
class Client {
public:
    // 无参数构造函数
    Client(int _port);
	void addfd(int& fd,bool isET); 
    // 连接服务器
    void Connect();
    // 断开连接
    void Close();
    // 启动客户端
    void Start();
private:
	int port;
    // 当前连接服务器端创建的socket
    int sock;
    // 当前进程ID
    int pid;
    // epoll_create创建后的返回值
    int epfd;
    // 创建管道，其中fd[0]用于父进程读，fd[1]用于子进程写
    int pipe_fd[2];
    // 表示客户端是否正常工作
    bool isClientwork;
    // 聊天信息缓冲区
    char message[BUF_SIZE];
    //用户连接的服务器 IP + port
    struct sockaddr_in Server;
};

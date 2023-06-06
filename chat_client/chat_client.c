#include <stdio.h>
#include <winsock2.h>
#include <pthread.h> 
#include <string.h> 
#include <stdlib.h>
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024
#define PORT 6666

void * recvmsg(void* arg)
{
    printf("多线程测试\n");
    SOCKET fd=*(int *)arg;
    while(1)
    {
        char buf[BUF_SIZE];
        int len=recv(fd,buf,sizeof(buf),0);
        if(NULL != strstr(buf,"人数已满"))
        {
            printf("%s\n",buf);
            closesocket(fd);
            pthread_exit(NULL);
        }
        if(len<0)
        {
            printf("与服务器连接中断...\n");
            closesocket(fd);
            pthread_exit(NULL);
        }
        else{
            printf("%s\n",buf);
        }
    }
    
}
/*
void * sendmsg(void *arg)
{
    printf("多线程测试22\n");
    SOCKET fd=*(int*)arg;
    while(1)
    {
        char buf[BUF_SIZE];
        gets(buf);
        int len=send(fd,buf,sizeof(buf),0);
        if(len<0)
        {
            printf("数据传送失败...\n");
            closesocket(fd);
            //break;
            pthread_exit(NULL);
        }
    }
    
}

*/

int main()
{
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2,2),&WSAData);

    SOCKET sockcl=socket(AF_INET,SOCK_STREAM,0);
    if(sockcl!=-1)
	{
		printf("创建套接字成功!\n");
    }
    else
    {
        printf("创建套接字失败!\n");
        return -1;
	}
    SOCKADDR_IN clientaddr;
	clientaddr.sin_addr.S_un.S_addr=inet_addr("127.0.0.1");
	clientaddr.sin_family=AF_INET;
	clientaddr.sin_port=htons(PORT);
    int a=connect(sockcl,(SOCKADDR *)&clientaddr,sizeof(clientaddr));	
    if(a<0)
    {
        printf("connect error\n");
        return -1;
    }
    else{
        printf("connect success\n");
    }

    char buf[BUF_SIZE];
    pthread_t rec;
    //pthread_t sen;
    pthread_create(&rec,NULL,recvmsg,&sockcl);
    //pthread_create(&sen,NULL,sendmsg,&sockcl);

    printf("多线程测试22\n");
    
    while(1)
    {
        char buf[BUF_SIZE];
        //recv(sockcl,buf,sizeof(buf),0);
        //printf("%s\n",buf);
        gets(buf);
        int len=send(sockcl,buf,sizeof(buf),0);
        if(len<0)
        {
            printf("数据传送失败...\n");
            closesocket(sockcl);
            //break;
            pthread_exit(NULL);
        }
    }

    WSACleanup();
    return 0;
}


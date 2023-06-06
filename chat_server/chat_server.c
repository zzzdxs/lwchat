#include <stdio.h>
#include <winsock2.h>
#include <pthread.h> 
#include <string.h> 
#include <stdlib.h>
#include <semaphore.h> //信号量
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024
#define PORT 6666
#define CLIENT_MAX 5  //聊天室最多人数
int sem=5;
//sem_t sem=(int *)CLIENT_MAX;
SOCKET msgsock;
int clientfd[CLIENT_MAX+1]={};//存储连接的客户端

void send_all(char *buf)//客户端接收到的消息发送到所有已连接客户端
{
    for(int i=0;i<CLIENT_MAX;i++)
    {
        if(clientfd[i]!=-1)
        {
            //printf("%s\n",buf);
            //printf("send to %d \n",clientfd[i]);
            send(clientfd[i],buf,strlen(buf)+1,0);
        }
    }
}

void *chat_init(void * arg)
//接入新客户端，在客户端退出前本线程一直运行，接收发送数据
//主程序运行时，有多个类似的线程同时进行
{
    //printf("新线程开启\n");
    sem--;
    //sem_wait(&sem);//可用信号量-1
    if(sem>=0)
    {
        SOCKET fd=*(int *)arg;
        char buf[BUF_SIZE];
        char name[BUF_SIZE],temp[BUF_SIZE];
        //printf("test1\n");
        sprintf(temp,"请输入用户ID: ");
        //printf("test2\n");
        int tt=send(fd,temp,strlen(temp),0);
        if(tt<0)
        {
            printf("send error\n");
        }
        recv(fd,name,sizeof(name),0);
        sprintf(temp,"------%s 进入群聊------",name);
        printf("%s\n",temp);
        send_all(temp);

        for(;;)
        {
            int len=recv(fd,buf,sizeof(buf),0);
            if(len<=0)//客户端退出
            {
                sprintf(temp,"------%s 退出群聊------",name);
                printf("%s\n",temp);
                //找出clientfd中对应的fd并删除
                for(int index=0;index<CLIENT_MAX;index++)
                {
                    if(clientfd[index]==fd)
                    {
                        clientfd[index]=-1;
                        break;
                    }
                }
                closesocket(fd);
                send_all(temp);

                //可用信号量+1
                //sem_post(&sem);
                sem++;
                pthread_exit(NULL);
            }
            else{
                sprintf(temp,"%s: %s",name,buf);
                //printf("%s\n",temp);
                send_all(temp);
            }
            
        }

    }

}

int main()
{
    WSADATA WSAData;
    WSAStartup(MAKEWORD(2,2),&WSAData);

    SOCKET serverfd=socket(AF_INET,SOCK_STREAM,0);
    if(serverfd!=-1)
    {
        printf("------聊天室已开启------\n");
    }

    struct  sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;//IPv4
    serveraddr.sin_port=htons(PORT);//端口号
    serveraddr.sin_addr.s_addr=INADDR_ANY;//任意IP
    bind(serverfd,(SOCKADDR *)&serveraddr,sizeof(serveraddr));//完成绑定

    listen(serverfd,5);
    printf("------等待用户进入------\n");

    memset(clientfd,-1,sizeof(clientfd));
    for(;;)
    {
        //首先连接客户端，判断是否超过允许的最大线程数后决定是否取消连接关闭客户端
        struct  sockaddr_in clientaddr;
        int len=sizeof(clientaddr);
        int i=0;
        while(clientfd[i]!=-1)
        {
            i++;
        }
        clientfd[i]=accept(serverfd,(SOCKADDR *)&clientaddr,&len);
        if(clientfd[i]<0)//判断是否连接成功
        {
            printf("accept error\n");
            break;
        }
        char buf[BUF_SIZE];
        if(sem<=0)//判断是否超过最大线程数
        {
            printf("聊天室人数已满，%d客户端连接失败...\n",clientfd[i]);
            sprintf(buf,"聊天室人数已满，%d客户端连接失败\n",clientfd[i]);
            send(clientfd[i],buf,BUF_SIZE,0);
            closesocket(clientfd[i]);
            clientfd[i]=-1;
        }
        /*
        else{
            printf("有新客户端进入聊天室\n");

        }
        */
        
        //开启多线程，每个线程线性进行，分别负责各个客户端的命名以及在聊天室发送接收数据
        pthread_t chat;
        pthread_create(&chat,NULL,chat_init,&clientfd[i]);
        /*
        int RecvLen=recv(clientfd,buf,BUF_SIZE,0);
        int SendLen=send(clientfd,buf,BUF_SIZE,0);
        */
        
    }
    WSACleanup();
    system("pause");
    return 0;
    
}
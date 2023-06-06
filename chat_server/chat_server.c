#include <stdio.h>
#include <winsock2.h>
#include <pthread.h> 
#include <string.h> 
#include <stdlib.h>
#include <semaphore.h> //�ź���
#pragma comment(lib,"ws2_32.lib")

#define BUF_SIZE 1024
#define PORT 6666
#define CLIENT_MAX 5  //�������������
int sem=5;
//sem_t sem=(int *)CLIENT_MAX;
SOCKET msgsock;
int clientfd[CLIENT_MAX+1]={};//�洢���ӵĿͻ���

void send_all(char *buf)//�ͻ��˽��յ�����Ϣ���͵����������ӿͻ���
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
//�����¿ͻ��ˣ��ڿͻ����˳�ǰ���߳�һֱ���У����շ�������
//����������ʱ���ж�����Ƶ��߳�ͬʱ����
{
    //printf("���߳̿���\n");
    sem--;
    //sem_wait(&sem);//�����ź���-1
    if(sem>=0)
    {
        SOCKET fd=*(int *)arg;
        char buf[BUF_SIZE];
        char name[BUF_SIZE],temp[BUF_SIZE];
        //printf("test1\n");
        sprintf(temp,"�������û�ID: ");
        //printf("test2\n");
        int tt=send(fd,temp,strlen(temp),0);
        if(tt<0)
        {
            printf("send error\n");
        }
        recv(fd,name,sizeof(name),0);
        sprintf(temp,"------%s ����Ⱥ��------",name);
        printf("%s\n",temp);
        send_all(temp);

        for(;;)
        {
            int len=recv(fd,buf,sizeof(buf),0);
            if(len<=0)//�ͻ����˳�
            {
                sprintf(temp,"------%s �˳�Ⱥ��------",name);
                printf("%s\n",temp);
                //�ҳ�clientfd�ж�Ӧ��fd��ɾ��
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

                //�����ź���+1
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
        printf("------�������ѿ���------\n");
    }

    struct  sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;//IPv4
    serveraddr.sin_port=htons(PORT);//�˿ں�
    serveraddr.sin_addr.s_addr=INADDR_ANY;//����IP
    bind(serverfd,(SOCKADDR *)&serveraddr,sizeof(serveraddr));//��ɰ�

    listen(serverfd,5);
    printf("------�ȴ��û�����------\n");

    memset(clientfd,-1,sizeof(clientfd));
    for(;;)
    {
        //�������ӿͻ��ˣ��ж��Ƿ񳬹����������߳���������Ƿ�ȡ�����ӹرտͻ���
        struct  sockaddr_in clientaddr;
        int len=sizeof(clientaddr);
        int i=0;
        while(clientfd[i]!=-1)
        {
            i++;
        }
        clientfd[i]=accept(serverfd,(SOCKADDR *)&clientaddr,&len);
        if(clientfd[i]<0)//�ж��Ƿ����ӳɹ�
        {
            printf("accept error\n");
            break;
        }
        char buf[BUF_SIZE];
        if(sem<=0)//�ж��Ƿ񳬹�����߳���
        {
            printf("����������������%d�ͻ�������ʧ��...\n",clientfd[i]);
            sprintf(buf,"����������������%d�ͻ�������ʧ��\n",clientfd[i]);
            send(clientfd[i],buf,BUF_SIZE,0);
            closesocket(clientfd[i]);
            clientfd[i]=-1;
        }
        /*
        else{
            printf("���¿ͻ��˽���������\n");

        }
        */
        
        //�������̣߳�ÿ���߳����Խ��У��ֱ�������ͻ��˵������Լ��������ҷ��ͽ�������
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
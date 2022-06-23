/*server.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <iostream>

#include "message.h"
#include "message_parser.h"

using namespace std;

#define BUFSIZE 4096
#define SERV_PORT 8000
#define OPEN_MAX 1024

int main()
{
  router r("root", "root123", "netdisk");

  int i, j, maxi, listenfd, connfd, sockfd;
  int nready, efd, res;
  ssize_t n;
  char buf[BUFSIZE], str[INET_ADDRSTRLEN];
  socklen_t clilen;
  int client[OPEN_MAX];
  struct sockaddr_in cliaddr, servaddr;

  struct epoll_event tep, ep[OPEN_MAX]; //�����¼�
  /*����һ������ͨ���׽��֣������ļ�������listenfd*/
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  /*��ʼ�� IP���� �˿�*/
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(SERV_PORT);
  /*��listenfd�󶨷���˵�ַ*/
  bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  /*��������*/
  listen(listenfd, 20);
  /*���ͻ��˱�ʶ��ʼ��Ϊ-1*/
  for (i = 0; i < OPEN_MAX; i++)
  {
    client[i] = -1;
  }
  maxi = -1;
  /*�����ں�Ҫ�������ļ����������� OPEN_MAX = 1024*/
  efd = epoll_create(OPEN_MAX);

  if (efd == -1)
  {
    perror("epoll_create");
  }

  tep.events = EPOLLIN;   /*�����ļ��������Ŀɶ��¼�*/
  tep.data.fd = listenfd; /*����Ϊ�������ļ�������*/
  /*����epoll��ص��ļ��������ϵ��¼�*/
  res = epoll_ctl(efd, EPOLL_CTL_ADD /*ע���µ�fd��efd*/, listenfd, &tep);

  if (res == -1)
    perror("epoll_ctl");

  for (;;)
  {
    /*�ȴ�������ļ������������¼��Ĳ�������������*/
    nready = epoll_wait(efd, ep, OPEN_MAX, -1);
    if (nready == -1)
      perror("epoll_wait");
    for (i = 0; i < nready; i++)
    {
      if (!(ep[i].events & EPOLLIN)) /*������EPOLLIN�¼����������µĴ���*/
        continue;
      if (ep[i].data.fd == listenfd)
      { /*����EPOLLIN��ִ�����ӣ���������*/
        clilen = sizeof(cliaddr);
        /*�������󣬷������ļ�������connfd����ͨ��*/
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("received from %s at PORT %d\n", (char *)inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));

        /*�������¿ͻ���������ͻ��˼���*/
        for (j = 0; j < OPEN_MAX; j++)
          if (client[j] < 0)
          {
            client[j] = connfd;
            break;
          }
        if (j == OPEN_MAX)
          perror("�ͻ��˳�������");

        if (j > maxi)
          maxi = j; //��֤maxiΪ����ļ�������

        tep.events = EPOLLIN;
        tep.data.fd = connfd;
        res = epoll_ctl(efd, EPOLL_CTL_ADD /*ע���µ�connfd��efd*/, connfd, &tep);
        if (res == -1)
          perror("epoll_ctl");
      }
      else
      { /*����efd�м����Ŀͻ�������*/
        sockfd = ep[i].data.fd;
        n = read(sockfd, buf, BUFSIZE);
        if (n == 0)
        { /*��ȡ��Ϊ��*/
          for (j = 0; j <= maxi; j++)
          {
            if (client[j] == sockfd)
            {
              client[j] = -1;
              break;
            }
          }
          /*�����sockfd�ļ��������¼��ļ���*/
          res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
          if (res == -1)
            perror("epoll_ctl");
          close(sockfd);
          printf("client[%d] closed connection\n", j);
        }
        else
        { /*�ǿ�����ͻ�����Ϣ*/
          auto res = r.parse(buf);
          write(sockfd, res.c_str(), res.size());
        }
      }
    }
  }
  /*�رռ���*/
  close(listenfd);
  close(efd);
  return 0;
}
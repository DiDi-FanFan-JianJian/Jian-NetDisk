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
#include "md5.h"

using namespace std;

#define BUFSIZE 1024 * 201
#define SERV_PORT 8000
#define OPEN_MAX 1024

const char* log_file = "/home/netdisk/db.log";

int main()
{
  string md5;
  get_file_md5("./server.cpp", md5);
  cout << md5 << endl;

  router r("root", "root123", "netdisk");

  int i, j, maxi, listenfd, connfd, sockfd;
  int nready, efd, res;
  ssize_t n;
  char buf[BUFSIZE], str[INET_ADDRSTRLEN];
  socklen_t clilen;
  int client[OPEN_MAX];
  struct sockaddr_in cliaddr, servaddr;

  struct epoll_event tep, ep[OPEN_MAX]; //监听事件
  /*分配一个网络通信套接字，监听文件描述符listenfd*/
  listenfd = socket(AF_INET, SOCK_STREAM, 0);
  /*初始化 IP类型 端口*/
  // string ip = "1.15.144.212";
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(SERV_PORT);
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

  // 端口重用
  int optval = 1;
  setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /*将listenfd绑定服务端地址*/
  bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  /*监听请求*/
  listen(listenfd, 20);
  /*将客户端标识初始化为-1*/
  for (i = 0; i < OPEN_MAX; i++)
  {
    client[i] = -1;
  }
  maxi = -1;
  /*告诉内核要监听的文件描述符个数 OPEN_MAX = 1024*/
  efd = epoll_create(OPEN_MAX);

  if (efd == -1)
  {
    perror("epoll_create");
  }

  tep.events = EPOLLIN;   /*监听文件描述符的可读事件*/
  tep.data.fd = listenfd; /*设置为监听的文件描述符*/
  /*控制epoll监控的文件描述符上的事件*/
  res = epoll_ctl(efd, EPOLL_CTL_ADD /*注册新的fd到efd*/, listenfd, &tep);

  if (res == -1)
    perror("epoll_ctl");

  for (;;)
  {
    /*等待所监控文件描述符上有事件的产生，阻塞监听*/
    nready = epoll_wait(efd, ep, OPEN_MAX, -1);
    if (nready == -1)
      perror("epoll_wait");
    for (i = 0; i < nready; i++)
    {
      if (!(ep[i].events & EPOLLIN)) /*若不是EPOLLIN事件，不做往下的处理*/
        continue;
      if (ep[i].data.fd == listenfd)
      { /*若是EPOLLIN，执行连接，接受请求*/
        clilen = sizeof(cliaddr);
        /*接受请求，分配新文件描述符connfd进行通信*/
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("received from %s at PORT %d\n", (char *)inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
        
        // 日志追加写
        FILE *fp = fopen(log_file, "a");
        // 时间 ip 端口
        time_t t = time(NULL);
        struct tm *tm = localtime(&t);
        fprintf(fp, "%d-%d-%d %d:%d:%d %s %d\n", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec, (char *)inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)), ntohs(cliaddr.sin_port));
        fclose(fp);

        /*若将此新客户端添加至客户端集中*/
        for (j = 0; j < OPEN_MAX; j++)
          if (client[j] < 0)
          {
            client[j] = connfd;
            break;
          }
        if (j == OPEN_MAX)
          perror("客户端超过限制");

        if (j > maxi)
          maxi = j; //保证maxi为最大文件描述符

        tep.events = EPOLLIN;
        tep.data.fd = connfd;
        res = epoll_ctl(efd, EPOLL_CTL_ADD /*注册新的connfd到efd*/, connfd, &tep);
        if (res == -1)
          perror("epoll_ctl");
      }
      else
      { /*处理efd中监听的客户端请求*/
        sockfd = ep[i].data.fd;
        n = read(sockfd, buf, BUFSIZE);
        if (n == 0)
        { /*读取若为空*/
          for (j = 0; j <= maxi; j++)
          {
            if (client[j] == sockfd)
            {
              client[j] = -1;
              break;
            }
          }
          /*清除对sockfd文件描述符事件的监听*/
          res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
          if (res == -1)
            perror("epoll_ctl");
          close(sockfd);
          printf("client[%d] closed connection\n", j);
        } 
        else
        { /*非空则处理客户端信息*/
          cout << "received from " << sockfd << ": " << (int)buf[0] << endl;
          auto res = r.parse(buf);
          write(sockfd, res.c_str(), res.size());
        }
      }
    }
  }
  /*关闭监听*/
  close(listenfd);
  close(efd);
  return 0;
}
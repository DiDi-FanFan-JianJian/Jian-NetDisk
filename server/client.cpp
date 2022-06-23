/*client.c*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <iostream>

#include "message.h"

using namespace std;

#define BUFSIZE 666
#define SERV_PORT 8000
#define OPEN_MAX 1024

int main(int argc, char *argv[])
{
  struct sockaddr_in servaddr;
  char buf[BUFSIZE];
  int sockfd, n;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);
  servaddr.sin_port = htons(SERV_PORT);

  /*连接服务端*/
  connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  while (true)
  {
    int type;
    string username, password;
    cin >> type >> username >> password;
    LoginMessage msg;
    strcpy(msg.username, username.c_str());
    strcpy(msg.password, password.c_str());
    buf[0] = type;
    memcpy(buf + 1, &msg, sizeof(msg));

    /*通过sockfd给服务端发送数据*/
    write(sockfd, buf, sizeof(msg) + 1);

    n = read(sockfd, buf, BUFSIZE);
    if (n == 0)
      printf("the other side has been closed.\n");
    else /*打印输出服务端传过来的数据*/
      write(STDOUT_FILENO, buf, n);
    cout << endl;
  }

  close(sockfd);
  return 0;
}
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
#include "md5.h"

using namespace std;

#define BUFSIZE 2048
#define SERV_PORT 8000
#define OPEN_MAX 1024

void upload_file(int sockfd)
{
  // 读文件
  FILE *fp = fopen("./test_file/test.txt", "r");
  int size = 0;
  // 获取文件大小
  fseek(fp, 0, SEEK_END);
  size = ftell(fp);

  int block_num = size / BLOCK_SIZE + (size % BLOCK_SIZE ? 1 : 0);
  int cur_idx = 1;
  string md5;
  get_file_md5("./test_file/test.txt", md5);
  char buf_send[BUFSIZE];
  while (true) {
    getchar();

    cout << "cur_idx: " << cur_idx << endl;
    cout << "block_num: " << block_num << endl;
    cout << "md5: " << md5 << endl;
    cout << endl;

    UploadFileMessage msg;
    strcpy(msg.md5, md5.c_str());
    msg.file_size = size;
    msg.block_num = block_num;
    msg.block_id = cur_idx;

    // 发送文件信息
    buf_send[0] = MSG_UPLOAD_FILE;
    memcpy(buf_send + 1, &msg, sizeof(msg));
    send(sockfd, buf_send, sizeof(msg) + 1, 0);
    
    cout << "send1" << endl;

    recv(sockfd, buf_send, sizeof(buf_send), 0);
    UploadFileResponse res(buf_send);

    cout << "res.checked " << res.checked << endl;
    cout << "res.block_id " << res.block_id << endl << endl;

    // 不成功就再次发送
    if (!res.checked) {
      cur_idx = res.block_id;
      continue;
    }

    // 读取文件内容
    UploadBlockMessage block_msg;
    memset(block_msg.block_data, 0, BLOCK_SIZE);
    fseek(fp, (cur_idx - 1) * BLOCK_SIZE, SEEK_SET);
    fread(block_msg.block_data, min(BLOCK_SIZE, size - (cur_idx - 1) * BLOCK_SIZE), 1, fp);
    strcpy(block_msg.md5, md5.c_str());
    block_msg.block_id = cur_idx;
    block_msg.size = min(BLOCK_SIZE, size - (cur_idx - 1) * BLOCK_SIZE);

    // 发送文件内容
    memset(buf_send, 0, sizeof(buf_send));
    buf_send[0] = MSG_UPLOAD_BLOCK;
    memcpy(buf_send + 1, &block_msg, sizeof(block_msg));
    send(sockfd, buf_send, sizeof(block_msg) + 1, 0);

    cout << "send2: " << min(BLOCK_SIZE, size - (cur_idx - 1) * BLOCK_SIZE) << endl;

    // 更新当前块索引
    recv(sockfd, buf_send, sizeof(buf_send), 0);
    UploadBlockResponse block_res(buf_send);
    cout << "block_res.next_block_id " << block_res.next_block_id << endl << endl;

    cur_idx = block_res.next_block_id;
    if (cur_idx == -1) {
      break;
    }
  }
}

void create_file_dir(int sockfd) 
{
  char send_buf[BUFSIZE];
  send_buf[0] = MSG_CREATE_FILE_DIR;
  CreateFileDirMessage msg;
  strcpy(msg.username, "123");
  strcpy(msg.md5, "8beb825ca302cc1abbaf14a164fa59b4");
  strcpy(msg.dir_name, "root");
  strcpy(msg.file_name, "test.txt");

  memcpy(send_buf + 1, &msg, sizeof(msg));
  send(sockfd, send_buf, sizeof(msg) + 1, 0);
  recv(sockfd, send_buf, sizeof(send_buf), 0);
  CreateFileDirResponse res(send_buf);
  cout << "res.status " << res.status << endl;
}

void create_dir(int sockfd, string dirname, string pdirname)
{
  char send_buf[BUFSIZE];
  send_buf[0] = MSG_CREATE_DIR;
  CreateDirMessage msg;
  strcpy(msg.username, "123");
  strcpy(msg.dirname, dirname.c_str());
  strcpy(msg.pdirname, pdirname.c_str());
  memcpy(send_buf + 1, &msg, sizeof(msg));
  send(sockfd, send_buf, sizeof(msg) + 1, 0);
  recv(sockfd, send_buf, sizeof(send_buf), 0);
  CreateDirResponse res(send_buf);
  cout << "res.status " << res.status << endl;
}

int main(int argc, char *argv[])
{
  struct sockaddr_in servaddr;
  int sockfd;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  inet_pton(AF_INET, "1.15.144.212", &servaddr.sin_addr);
  servaddr.sin_port = htons(SERV_PORT);

  /*连接服务端*/
  connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

  // upload_file(sockfd);
  // create_file_dir(sockfd);
  create_dir(sockfd, "home", "root");
  create_dir(sockfd, "etc", "root");

  close(sockfd);
  return 0;
}
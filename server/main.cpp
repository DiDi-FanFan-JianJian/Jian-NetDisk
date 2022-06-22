/*
 * @Author: Dino
 * @Date: 2022-06-20 19:34:23
 * @LastEditors: Dino
 * @LastEditTime: 2022-06-22 16:13:07
 * @Description: socket 基类
 */
#include "SockBase.h"

SockBase::SockBase(): sockfd(-1)
{
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (!is_valid())
    throw SocketException("Socket creation failed (socket() returned -1)");
  // 接收数据超时时间
  struct timeval timeout = {1, 0};
  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
}

void SockBase::connect(const int port, const std::string& ip)
{
  if (!is_valid())
    throw SocketException("Socket is not valid");
  
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  // ip为网址
  if (inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) <= 0) {
    struct hostent* host = gethostbyname(ip.c_str());
    if (host == NULL)
      throw SocketException("Unknown host");
    addr.sin_addr = *(struct in_addr*)host->h_addr;
  }
  if (::connect(sockfd, (sockaddr*)&addr, sizeof(addr)) < 0)
    throw SocketException("Connect failed");
}

void SockBase::send(const std::string& msg) const
{
  if (!is_valid())
    throw SocketException("Socket is not valid");
  if (::send(sockfd, msg.c_str(), msg.size(), 0) < 0)
    throw SocketException("Send failed");
}

std::string SockBase::recv(size_t max) const
{
  if (!is_valid())
    throw SocketException("Socket is not valid");

  char buf[max];
  memset(buf, 0, max);
  if (::recv(sockfd, buf, max, 0) == -1) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
    }
    else
      throw SocketException("Recv failed");
  }
  return std::string(buf);
}

SockBase::~SockBase()
{
  if (is_valid())
    ::close(sockfd);
}
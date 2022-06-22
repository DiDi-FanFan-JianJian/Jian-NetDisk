/*
 * @Author: Dino
 * @Date: 2022-06-20 19:14:00
 * @LastEditors: Dino
 * @LastEditTime: 2022-06-20 19:14:00
 * @Description: 
 */
#include <HttpUtils.h>
#include <fstream>

namespace http {

bool is_valid_url(const std::string& url)
{
  std::regex pattern(R"(^(?:http(s)?:\/\/)?[\w.-]+(?:\.[\w\.-]+)+[\w\-\._~:/?#%[\]@!\$&'\*\+,;=.]+$)");
  return std::regex_match(url, pattern);
}

bool is_valid_ip(const std::string& ip)
{
  std::regex pattern("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
  return std::regex_match(ip, pattern);
}

// 根据url获取ip
std::string get_ip_from_url(const std::string& url, const int port)
{
  // 是否是ip地址
  if (is_valid_ip(url)) {
    return url;
  }

  // 获取ip
  struct addrinfo hints;
  struct addrinfo *result;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICSERV;
  int ret = getaddrinfo(url.c_str(), std::to_string(port).c_str(), &hints, &result);
  if (ret != 0) {
    return "";
  }
  return inet_ntoa(((struct sockaddr_in*)result->ai_addr)->sin_addr);
}

// 获取文件二进制流
std::string get_file_content(const std::string& filepath)
{
  std::ifstream ifs(filepath, std::ios::binary);
  if (!ifs) {
    return "";
  }
  std::stringstream ss;
  ss << ifs.rdbuf();
  return ss.str();
}

std::string get_boundary()
{
  std::string boundary = "----WebKitFormBoundary";
  for (int i = 0; i < 10; i++) {
    boundary += std::to_string(rand() % 10);
  }
  return boundary;
}

} // namespace http
/*
 * @Author: Dino
 * @Date: 2022-06-20 19:10:23
 * @LastEditors: Dino
 * @LastEditTime: 2022-06-20 19:14:22
 * @Description: http客户端
 */

#include <HttpClient.h>

HttpResponse HttpClient::get(const std::string& url, const int port, const std::string path, const HttpGetParams params, HttpHeader header)
{
  auto ip = http::get_ip_from_url(url, port);
  if (ip.empty()) {
    throw SocketException("invalid url");
  }
  auto&& req = make_request(ip, port, header);
  req.set_path(path, params);
  req.set_method("GET");
  req.set_host(url);
  req.set_port(port);
  return send_request(req);
}

HttpResponse HttpClient::post(const std::string& url, const int port, const std::string path, const HttpPostBody data, HttpHeader header)
{
  auto ip = http::get_ip_from_url(url, port);
  if (ip.empty()) {
    throw SocketException("invalid url");
  }
  std::stringstream ss;
  if (!data.empty()) {  
    for (auto&& param : data) {
      ss << param.first << "=" << param.second << "&";
    }
  }

  header["Content-Type"] = "application/x-www-form-urlencoded";
  header["Content-Length"] = std::to_string(ss.str().size() - 1);
  auto&& req = make_request(ip, port, header);
  req.set_path(path);
  req.set_method("POST");
  req.set_host(url);
  req.set_port(port);
  
  req.set_body(ss.str());
  req.body.erase(req.body.end() - 1);
  return send_request(req);
}

HttpResponse HttpClient::post(const std::string& url, const int port, const std::string path, HttpFileForms& forms, HttpHeader header)
{
  auto ip = http::get_ip_from_url(url, port);
  if (ip.empty()) {
    throw SocketException("invalid url");
  }

  // 生成body
  auto boundary = http::get_boundary();
  std::stringstream ss;
  for (auto&& form : forms) {
    ss << "--" << boundary << CRLF;
    ss << "Content-Disposition: form-data; name=\"" << form.name << "\"";
    if (form.filename != "") {
      if (form.filename == "none")
        form.filename = "";
      ss << "; filename=\"" << form.filename << "\"";
      ss << CRLF << "Content-Type: " << form.content_type << CRLF << CRLF;
      ss << form.content << CRLF;
    }
    else {
      ss << CRLF << CRLF;
      ss << form.content << CRLF;
    }
  }
  ss << "--" << boundary << "--" << CRLF;

  // header
  header["Content-Type"] = "multipart/form-data; boundary=" + boundary;
  header["Content-Length"] = std::to_string(ss.str().size());
  auto&& req = make_request(ip, port, header);

  req.set_path(path);
  req.set_method("POST");
  req.set_host(url);
  req.set_port(port);

  req.set_body(ss.str());

  return send_request(req);
}

HttpRequest HttpClient::make_request(const std::string& ip, const int& port, HttpHeader header)
{
  HttpRequest request;
  header["host"] = ip + ":" + std::to_string(port);
  header["user-agent"] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/101.0.4951.64 Safari/537.36";
  header["accept"] = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9";
  request.set_header(header);
  request.set_host(ip);
  request.set_port(port);

  return request;
}

HttpResponse HttpClient::send_request(const HttpRequest& request)
{
  std::stringstream header_stream;
  //  请求行 POST /?id=1 HTTP/1.1
  header_stream << request.method << " " \
                << request.path << " " \
                << "HTTP/1.1" << CRLF;
  // 头部
  for (auto&& header : request.header) {
    header_stream << header.first << ": " << header.second << CRLF;
  }
  // 空行
  header_stream << CRLF;
  // body
  header_stream << request.body;

  // std::cout << header_stream.str() << std::endl;
  HttpResponse response;

  // 发送请求
  send(header_stream.str());

  // 接收响应
  std::stringstream response_stream;

  while (true) {
    auto data = recv(MAX_RECV);
    response_stream << data;
    if (data.empty()) {
      break;
    }
  }
  response.set_body(response_stream.str());
  return response;
}

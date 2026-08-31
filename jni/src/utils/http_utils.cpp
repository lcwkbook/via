#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h> // 用于 getaddrinfo
#include <sstream>
#include <memory> // 添加智能指针支持
#include "http_check.h" // 包含 parse_host_port 函数定义

std::string send_post(const std::string& host_port, const std::string& endpoint, const std::string& params) {
    // 解析主机和端口
    auto host_port_pair = parse_host_port(host_port);
    std::string host = host_port_pair.first;
    int port = host_port_pair.second;
    
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return "";
    }

    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // 将端口转换为字符串
    std::string port_str = std::to_string(port);
    
    int status = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo failed: " << gai_strerror(status) << std::endl;
        close(sock);
        return "";
    }

    std::unique_ptr<addrinfo, decltype(&freeaddrinfo)> res_ptr(res, freeaddrinfo);
    struct sockaddr_in *server_addr = (struct sockaddr_in *)res->ai_addr;
    server_addr->sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)server_addr, sizeof(*server_addr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        close(sock);
        return "";
    }

    // Host头使用原始host_port（包含端口信息）
    std::string request = "POST " + endpoint + " HTTP/1.1\r\n" +
                          "Host: " + host_port + "\r\n" +
                          "Content-Type: application/x-www-form-urlencoded\r\n" +
                          "Content-Length: " + std::to_string(params.length()) + "\r\n" +
                          "Connection: close\r\n\r\n" +
                          params;

    if (send(sock, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "Send failed" << std::endl;
        close(sock);
        return "";
    }

    std::string response;
    char buffer[4096];
    int bytes_received;
    while ((bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        response += buffer;
    }

    // 提取响应主体
    std::string::size_type pos = response.find("\r\n\r\n");
    if (pos != std::string::npos) {
        response = response.substr(pos + 4);
    }

    return response;
} 
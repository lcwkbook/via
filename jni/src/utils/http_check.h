#ifndef HTTP_CHECK_H
#define HTTP_CHECK_H

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <utility> // for std::pair

// 解析主机和端口，支持 "host" 或 "host:port" 格式
// 返回: pair<host, port>
inline std::pair<std::string, int> parse_host_port(const std::string& host_port) {
    std::string::size_type colon_pos = host_port.find(':');
    if (colon_pos != std::string::npos) {
        std::string host = host_port.substr(0, colon_pos);
        std::string port_str = host_port.substr(colon_pos + 1);
        int port = std::atoi(port_str.c_str());
        if (port <= 0 || port > 65535) {
            port = 80; // 默认端口
        }
        return std::make_pair(host, port);
    } else {
        return std::make_pair(host_port, 80); // 默认端口80
    }
}

// 发送HTTP请求并获取状态码
int checkHttpStatusCode(const std::string& host_port) {
    // 解析主机和端口
    auto host_port_pair = parse_host_port(host_port);
    std::string host = host_port_pair.first;
    int port = host_port_pair.second;
    
    int sockfd;
    struct sockaddr_in serveraddr;
    socklen_t addrlen = sizeof(serveraddr);
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cerr << "socket: " << strerror(errno) << std::endl;
        return -1;
    }
    std::memset(&serveraddr, 0, addrlen);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    
    // 检查是否是IP地址格式
    struct in_addr addr;
    if (inet_aton(host.c_str(), &addr) != 0) {
        // 是IP地址，直接使用
        serveraddr.sin_addr = addr;
    } else {
        // 是域名，需要解析
        struct hostent* host_ent;
        host_ent = gethostbyname(host.c_str());
        if (host_ent == NULL) {
            std::cerr << "gethostbyname: " << strerror(h_errno) << std::endl;
            close(sockfd);
            return -1;
        }
        struct in_addr ip = *((struct in_addr *)host_ent->h_addr);
        serveraddr.sin_addr = ip;
    }
    
    if (connect(sockfd, (struct sockaddr *)&serveraddr, addrlen) < 0) {
        std::cerr << "connect: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1;
    }
    
    // Host头使用原始host_port（包含端口信息）
    std::string request = "GET / HTTP/1.1\r\nHost: " + host_port + "\r\nConnection: close\r\n\r\n";
    if (send(sockfd, request.c_str(), request.length(), 0) < 0) {
        std::cerr << "send: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1;
    }
    char buffer[1024];
    int bytes_read = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        std::cerr << "recv: " << strerror(errno) << std::endl;
        close(sockfd);
        return -1;
    }
    buffer[bytes_read] = '\0';
    // 从响应中提取状态码
    const char* status_code_start = std::strstr(buffer, "HTTP/1.1 ");
    if (status_code_start == NULL) {
        std::cerr << "Failed to find status code" << std::endl;
        close(sockfd);
        return -1;
    }
    status_code_start += 9;
    int status_code = std::atoi(status_code_start);
    close(sockfd);
    return status_code;
}

#endif // HTTP_CHECK_H
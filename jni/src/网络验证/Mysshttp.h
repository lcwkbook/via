#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifdef __cplusplus
extern "C"{
#endif

char* My_strstrstr(char* str, char* text, char* rear);//变态级的函数,看了可能会崩溃
char* httppost(char* hostname, char* url, char* cs);//POST方法
char* httpget(const char* hostname, char* url);//GET方法
char* getip(char* hostname);//域名转换IP
int hextoint(char* hex);//16进制字符串转整数

#ifdef __cplusplus
}
#endif



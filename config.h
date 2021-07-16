#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// server
#define CLIENTMAX 100 // 最大连接客户数

// both
#define BUF_SIZE 100       // 缓冲区大小 = BUF_SIZE - 1
#define DOMAIN "localhost" // 域名
#define PORT 8080          // 端口

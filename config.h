#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

// server
#define CLIENTMAX 100 // 最大连接客户数

// client
#define CHESSA 'O' // 玩家1的棋子的显示
#define CHESSB 'X' // 玩家2的棋子的显示

// both
#define BUF_SIZE 100       // 缓冲区大小 = BUF_SIZE - 1
#define DOMAIN "localhost" // 域名
#define PORT 8080          // 端口

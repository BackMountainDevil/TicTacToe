#include "client.h"

/* 多线程 I/O 分离 群聊客户端 */
char bufSend[BUF_SIZE] = {0};
char bufRecv[BUF_SIZE] = {0};
int vssocket;

// 创建套接字
Client::Client() {
  // 由域名获取 IP
  struct hostent *host = gethostbyname(DOMAIN);
  if (!host) {
    perror("Get IP address error!");
    exit(EXIT_FAILURE);
  }

  memset(&this->serv_addr, 0, sizeof(this->serv_addr)); //每个字节都用0填充
  this->serv_addr.sin_family = AF_INET;                 //使用IPv4地址
  this->serv_addr.sin_port = htons(PORT);               //端口

  if (inet_pton(AF_INET, inet_ntoa(*(struct in_addr *)host->h_addr_list[0]),
                &this->serv_addr.sin_addr) <= 0) { // IP 地址绑定和检查
    perror("Error: Invalid address/ Address not supported");
    exit(EXIT_FAILURE);
  }

  // 创建套接字
  this->sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock < 0) {
    perror("Error: Socket creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }
}

// 连接服务器
bool Client::Connect() {
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error: Connection creation failed");
    close(sock);
    return false;
  } else {
    return true;
  }
}

// 开始游戏
bool Client::Start() {
  if (this->Connect()) {
    if (read(sock, bufRecv, sizeof(bufRecv)) == -1) {
      perror("套接字已被关闭 read");
      close(sock);
      return false;
    } else {
      std::cout << bufRecv << std::endl;
    }
    while (true) {
      std::cin.getline(bufSend, BUF_SIZE);
      if (!strcmp(bufSend, "\\q")) { // 输入 ‘\q’ ,逐步终止程序
        shutdown(sock, SHUT_WR);
        read(sock, bufSend, sizeof(bufSend));
        break;
      }

      if (write(sock, bufSend, sizeof(bufSend)) == -1) { // 发送数据
        perror("套接字已被关闭 write");
        close(sock);
        return false;
      }

      if (read(sock, bufRecv, sizeof(bufRecv)) == -1) {
        perror("套接字已被关闭 read");
        close(sock);
        return false;
      } else {
        std::cout << bufRecv << std::endl;
      }
    }
  } else {
    // AI
  }

  return false;
}

// 关闭套接字
void Client::Close() { close(this->sock); }

int main() {
  Client cli;
  cli.Start();
  return 0;
}
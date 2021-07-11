#include "config.h"
#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netdb.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

pthread_mutex_t mutex; // 互斥量
char bufSend[BUF_SIZE];

struct Clients {
  int sum = 0;               // 客户数
  int clnt_socks[CLIENTMAX]; // 保存客户套接字的数组
  char status[CLIENTMAX]; // 'G' 游戏中, 'W' 未匹配队友，'F' 不在线
} cli;

void *send_msg(char *msg, int len, int fd = -1) {
  int num;
  if (fd != -1) {
    num = write(fd, msg, len);
    if (num < 0) {
      perror("Error: Send fail");
      return NULL;
    }
  } else {                      // 把消息发送给所有客户
    pthread_mutex_lock(&mutex); // 加锁
    for (int j = 0; j < cli.sum; j++) {
      num = write(cli.clnt_socks[j], msg, len);
      if (num < 0) {
        perror("Error: Send fail");
        return NULL;
      }
    }
    pthread_mutex_unlock(&mutex); // 解锁
  }
  return NULL;
}

// 处理新客户连接
void *handle_client(void *arg) {
  int clnt_sock = *((int *)arg);
  int recv_num = 0;

  while ((recv_num = read(clnt_sock, bufSend, sizeof(bufSend))) != 0) {
    std::cout << clnt_sock << " : " << bufSend << std::endl;
    if (bufSend[0] == '\\') {        //特殊指令
      if (!strcmp(bufSend, "\\Q")) { // 查询全部在线且不在游戏中的用户
        int pos = 0;
        pthread_mutex_lock(&mutex); // 加锁
        for (int i = 0; i < cli.sum; i++) {
          pos += sprintf(bufSend + pos, "%d ", cli.clnt_socks[i]);
        }
        bufSend[pos - 1] = '\n';      // 最后一个空格换成 换行符号
        pthread_mutex_unlock(&mutex); // 解锁
        send_msg(bufSend, recv_num, clnt_sock);
      }
    } else {
      send_msg(bufSend, recv_num);
    }
  }

  pthread_mutex_lock(&mutex);         // 加锁
  for (int i = 0; i < cli.sum; i++) { // 剔除掉线用户
    if (cli.clnt_socks[i] == clnt_sock) {
      while (i++ < cli.sum - 1) {
        cli.clnt_socks[i] = cli.clnt_socks[i + 1];
      }
      std::cout << "Client " << clnt_sock << " disconnect" << std::endl;
      cli.sum--; // 客户数量减一
      cli.status[clnt_sock] = 'F';
      break;
    }
  }
  pthread_mutex_unlock(&mutex); // 解锁
  close(clnt_sock);
  return NULL;
}

int main() {
  memset(&cli.status, 'F', CLIENTMAX); // 每个字节都用F填充

  struct hostent *host = gethostbyname(DOMAIN); // 由域名获取 IP
  if (!host) {
    perror("Get IP address error!");
    return -1;
  }

  int serv_sock = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
  if (serv_sock == -1) {
    perror("Error: Socket creation failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  int opt = 1;
  if (setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    perror("Setsockopt: 设置 地址/端口 可复用失败");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充
  serv_addr.sin_family = AF_INET;           // 使用IPv4地址
  serv_addr.sin_addr.s_addr =
      inet_addr(inet_ntoa(*(struct in_addr *)host->h_addr_list[0])); // IP地址
  serv_addr.sin_port = htons(PORT);                                  // 端口

  if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
    perror("Bind failed");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }

  // 进入监听状态，等待用户发起请求
  if (listen(serv_sock, CLIENTMAX) == -1) {
    perror("Listen");
    close(serv_sock);
    exit(EXIT_FAILURE);
  }
  printf("Waiting for connecting\n");

  pthread_mutex_init(&mutex, NULL); // 建立互斥量
  pthread_t t_id;

  struct sockaddr_in clnt_addr;
  socklen_t clnt_addr_size = sizeof(clnt_addr);
  while (true) {
    int clnt_sock =
        accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
      perror("Accept Error");
      continue;
    } else {
      pthread_mutex_lock(&mutex); // 加锁
      cli.clnt_socks[cli.sum++] = clnt_sock;
      cli.status[clnt_sock] = 'W';
      pthread_mutex_unlock(&mutex); // 解锁

      sprintf(bufSend, "\\%d", clnt_sock); // 告诉用户自己的套接字号
      send_msg(bufSend, sizeof(bufSend), clnt_sock);

      // 创建线程
      if (pthread_create(&t_id, NULL, handle_client, (void *)&clnt_sock) != 0) {
        perror("error - pthread_create");
        return -1;
      }
      pthread_detach(t_id); // 销毁进程

      std::cout << "New client：" << clnt_sock << " , IP "
                << inet_ntoa(clnt_addr.sin_addr) << " , Port "
                << ntohs(clnt_addr.sin_port) << std::endl;
    }
  }

  close(serv_sock);
  puts("Server close");
  return 0;
}

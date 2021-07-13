#ifndef CLIENT
#define CLIENT
#include "config.h"

class Client {
public:
  Client();         // 创建套接字
  void showBoard(); // 显示棋盘以及棋子
  int checkInput(); // 获取用户的合法输入
  int checkWinner(); // 检测赢家，返回值 -1（无赢家） or 1（玩家一） or
  // 2（玩家二）
  void clearScreen(); // 清空控制台所有内容, linux 与 windows 有所不同
  int AIInput();      // AI 输入
  bool Connect();     // 连接服务器
  bool Start();       // 开始游戏
  void PlayAI();      // 人机对战
  void Reset();       // 重置棋盘
  void Close();       // 关闭套接字

private:
  int BOARD[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int sock;
  char bufSend[BUF_SIZE];
  char bufRecv[BUF_SIZE];
  struct sockaddr_in serv_addr;
};

#endif
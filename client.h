#ifndef CLIENT
#define CLIENT
#include "config.h"

class Client {
public:
  Client(); // 创建套接字
  void showBoard(unsigned int bckgcolor = 47, unsigned int fregcolor = 30,
                 unsigned int Acolor = 32, unsigned int Bcolor = 35,
                 float rowscale = 0.2,
                 float colscale = 0.3); // 显示棋盘以及棋子
  int checkInput();                     // 获取用户的合法输入
  int checkWinner();                    // 检测赢家
  void ShowWinner(int winner, bool isfirst = true); // 输出赢家输家
  void clearScreen(); // 清空控制台所有内容
  int AIInput();      // AI 输入
  bool Connect();     // 连接服务器
  bool Start();       // 开始游戏
  void PlayAI();      // 人机对战
  void Reset();       // 重置棋盘
  void Close();       // 关闭套接字
  void ResetColor();  //  取消前面的 VT 控制码设置
  void SetColor(unsigned int bckgcolor,
                unsigned int fregcolor); // 初始化背景色和前景色。
  void ShowMenu(const char **menu, unsigned int size, unsigned int index,
                unsigned int bckgcolor = 47, unsigned int fregcolor = 30,
                unsigned int tilcolor = 32, unsigned int indexcolor = 34,
                float rowscale = 0.2,
                float colscale = 0.3); // 显示用户菜单。
  int GetMenuInput(unsigned int *index, unsigned int start,
                   unsigned int end); // 获取用户输入的 ASCII

private:
  int BOARD[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
  int sock;
  char bufSend[BUF_SIZE];
  char bufRecv[BUF_SIZE];
  struct sockaddr_in serv_addr;
  unsigned int menuindex = 1; // 默认菜单选项
  struct winsize windows;     // 终端窗口大小
  enum {                      // 可操作的按键
    UP = 119,                 // W
    DOWN = 115,               // S
    ENTER = 10,               // Enter
  };
};

#endif
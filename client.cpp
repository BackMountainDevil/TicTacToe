#include "client.h"

// 菜单
const char *oflinemenu[] = {"游戏菜单", "1. 重新连接服务器", "2. AI 模式",
                            "3. 退出游戏"};

// 可操作的按键
enum {
  UP = 119,   // W
  DOWN = 115, // S
  ENTER = 10, // Enter
};
/* 获取用户输入的 ASCII。
index: 菜单项对应的变量的地址
start：菜单项的最小值
end： 菜单项的最大值 */
int Client::GetMenuInput(unsigned int *index, unsigned int start,
                         unsigned int end) {
  system("stty -icanon"); // 关闭缓冲区，输入字符无需按回车键
  char command;
  command = getchar();
  system("stty icanon"); // 恢复规范输入
  switch (command) {
  case UP:
    if (*index > start)
      *index -= 1;
    break;
  case DOWN:
    if (*index < end)
      *index += 1;
    break;
  case ENTER:
    return ENTER;
  }

  return command;
}

/* 显示用户菜单。
menu：菜单标题和菜单子项
size：菜单子项的数量
index：当前被选中的菜单项
bckgcolor：背景色，数值参考 VT控制码 的颜色问题
fregcolor：前景色
tilcolor：标题颜色
indexcolor：被选中的菜单项的颜色
menu 从零开始计数,无标题的话 index 和 i 上界都要要减一，但我这里默认有标题*/
void Client::ShowMenu(const char **menu, unsigned int size, unsigned int index,
                      unsigned int bckgcolor, unsigned int fregcolor,
                      unsigned int tilcolor, unsigned int indexcolor,
                      struct winsize windows, float rowscale, float colscale) {
  unsigned int posrow = rowscale * windows.ws_row;
  unsigned int poscol = colscale * windows.ws_col;
  system("clear"); // 清空控制台
  for (unsigned int i = 0; i <= size; i++) {
    if (i == index) {             // 被选中的菜单子项
      printf("\033[%dC", poscol); // 光标右移动
      printf("\033[%d;%dm", bckgcolor, indexcolor);
      printf("\t->%s\n", menu[i]);
      printf("\033[%d;%dm", bckgcolor, fregcolor);
    } else if (i == 0) {                          // 菜单标题
      printf("\033[%dB\033[%dC", posrow, poscol); // 光标下移、右移动
      printf("\033[%d;%dm", bckgcolor, tilcolor);
      printf("\t  %s\n", menu[i]);
      printf("\033[%d;%dm", bckgcolor, fregcolor);
    } else {                      // 未选中的菜单子项
      printf("\033[%dC", poscol); // 光标右移动
      printf("\t  %s\n", menu[i]);
    }
  }
}

/* 初始化背景色和前景色。
设置主要是为了刚开始全终端都是背景色，不设置的话只有一小块是背景色很突兀，
虽然这突兀的情况在用户输入一次后就会消失
bckgcolor：背景色，数值参考 VT控制码 的颜色问题
fregcolor：前景色
 */
void Client::SetColor(unsigned int bckgcolor, unsigned int fregcolor) {
  printf("\033[%d;%dm", bckgcolor, fregcolor);
  printf("\033[2J"); // 清空控制台，和 system("clear"); 效果还是有点区别的
}

void Client::ResetColor() { printf("\033[0m"); }

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
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &windows); // 获取终端的行数、列数（高、宽）
  SetColor(47, 30); // 背景色 47-白色，前景色（字体颜色） 30-黑色
}

bool Client::Connect() {
  // 创建套接字
  this->sock = socket(AF_INET, SOCK_STREAM, 0);
  if (this->sock < 0) {
    perror("Error: Socket creation failed");
    close(sock);
    exit(EXIT_FAILURE);
  }
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    // perror("Error: Connection creation failed");
    puts("Unable to connect to Server. ");
    close(sock);
    return false;
  } else {
    return true;
  }
}

bool Client::Start() {
  bool iscon = false;
  bool isFinish = false;
  while (!isFinish) {
    iscon = this->Connect();
    if (iscon) {
      // 接收自己的序号
      if (read(sock, bufRecv, sizeof(bufRecv)) == -1) {
        perror("套接字已被关闭 read");
        close(sock);
        return false;
      } else {
        std::cout << bufRecv << std::endl;
      }
      std::cout << "connect to Server.\n"
                << "'\\S' to start 匹配. \n"
                << "'\\Q' to query. \n"
                << "'ai' to play with AI.\n"
                << "'\\q' to exit" << std::endl;

      while (true) {
        std::cin.getline(bufSend, BUF_SIZE);
        if (!strcmp(bufSend, "ai")) {
          this->PlayAI();
        } else if (!strcmp(bufSend, "\\q")) { // 输入 ‘\q’ ,逐步终止程序
          shutdown(sock, SHUT_WR);
          read(sock, bufSend, sizeof(bufSend));
          return 0;
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
        } else if (bufRecv[0] == '\\' &&
                   bufRecv[1] == 'G') { // 匹配到可能的对手
          std::cout << bufRecv << " | ";
          char *result = NULL;
          result = strtok(bufRecv, " "); // \G
          result = strtok(NULL, " ");    // 目标套接字
          int target = atoi(result);
          std::cout << target << std::endl;
          result = strtok(NULL, " "); // 指令，是否先手
          bool isfirst = false;
          if (!strcmp(result, "step1")) {
            puts("YOu first");
            isfirst = true;
          }
          // 开始游戏
          showBoard();
          int winner = -1;
          // 默认 先手 1 是自己，后手 2 是自己
          int count = 0, turn;
          while (count < 9) {
            int pos;
            turn = count % 2 + 1;
            if (turn == 2) {
              if (isfirst) { // 先手等待
                read(sock, bufRecv, sizeof(bufRecv));
                result = NULL;
                result = strtok(bufRecv, " "); // \p
                result = strtok(NULL, " ");    // 套接字
                result = strtok(NULL, " ");    //
                pos = atoi(result);

              } else { // 后手下棋
                std::cout << "You turn: ";
                pos = checkInput();
                std::sprintf(bufSend, "\\p %d %d", target,
                             pos); // 向对方发送自己的棋
                write(sock, bufSend, sizeof(bufSend));
              }
            } else {         // turn 1
              if (isfirst) { // 先手
                std::cout << "You turn: ";
                pos = checkInput();
                std::sprintf(bufSend, "\\p %d %d", target,
                             pos); // 向对方发送自己的棋
                write(sock, bufSend, sizeof(bufSend));
              } else { // 后手等待
                read(sock, bufRecv, sizeof(bufRecv));
                result = NULL;
                result = strtok(bufRecv, " "); // \p
                result = strtok(NULL, " ");    // 套接字
                result = strtok(NULL, " ");    //
                pos = atoi(result);
              }
            }

            BOARD[pos] = turn;
            clearScreen();
            showBoard();
            winner = checkWinner();
            if (winner > -1) {
              break;
            }
            count++;
          }
          switch (winner) {
          case 1:
            if (isfirst) {
              std::cout << "YOU WIN!" << std::endl;
            } else {
              std::cout << "you lose!" << std::endl;
            }
            break;
          case 2:
            if (isfirst) {
              std::cout << "you lose!" << std::endl;
            } else {
              std::cout << "YOU WIN!" << std::endl;
            }
            break;
          case 0:
            std::cout << "Nobody win" << std::endl;
            break;
          }
          std::cout << "Game Over. Take a rest" << std::endl;
          this->Reset();                // 重置棋盘
          std::sprintf(bufSend, "\\W"); // 告诉服务器我游戏结束了
          write(sock, bufSend, sizeof(bufSend));
          read(sock, bufRecv, sizeof(bufRecv));
        } else {
          std::cout << bufRecv << std::endl;
        }
      }
    } else { // 连接失败的时候
      ShowMenu(oflinemenu, 3, menuindex, 47, 30, 32, 34, windows, 0.2, 0.4);
      if (GetMenuInput(&menuindex, 1, 3) == ENTER) {
        if (menuindex == 3) {
          break;
        } else if (menuindex == 2) {
          this->PlayAI();
        }
      }
    }
  }

  return false;
}

void Client::showBoard() {
  // 显示棋盘以及棋子
  for (int i = 0; i < 9; i++) {
    std::cout << " __";
    if (BOARD[i] == 0) {
      std::cout << i;
    } else {
      if (BOARD[i] == 1) {
        std::cout << 'x';
      } else {
        std::cout << 'A';
      }
    }

    if ((i + 1) % 3 == 0) {
      std::cout << "__" << std::endl;
    } else {
      std::cout << "__ |";
    }
  }
}

int Client::checkInput() {
  int pos = -1;
  while (pos < 0 || pos > 8) {
    std::cin >> pos;
    if (BOARD[pos] != 0) {
      std::cout << "Position is not null, try again" << std::endl;
      pos = -1;
    }
  }
  return pos;
}

int Client::checkWinner() {
  // 检测赢家，返回值 -1（无赢家） or 1（玩家一） or 2（玩家二）or 0 （平手）
  int winner = -1;
  for (int i = 0; i < 3; i++) {
    if (BOARD[i] == BOARD[i + 3] && BOARD[i + 3] == BOARD[i + 6] &&
        BOARD[i] != 0) { // 竖
      winner = BOARD[i];
      break;
    } else if (BOARD[i * 3] == BOARD[i * 3 + 1] &&
               BOARD[i * 3 + 1] == BOARD[i * 3 + 2] &&
               BOARD[i * 3] != 0) { // 横
      winner = BOARD[i * 3];
      break;
    }
  }
  // 对角线
  if (BOARD[0] == BOARD[4] && BOARD[4] == BOARD[8] && BOARD[4] != 0) {
    winner = BOARD[4];
  } else if (BOARD[2] == BOARD[4] && BOARD[4] == BOARD[6] && BOARD[4] != 0) {
    winner = BOARD[4];
  }

  if (winner == -1) { // 检查是否平手
    winner = 0;       // 假设平手
    for (unsigned int i = 0; i < 9; i++) {
      if (BOARD[i] == 0) { // 有空位置没下，推翻假设
        winner = -1;
        break;
      }
    }
  }
  return winner;
}

void Client::clearScreen() { system("clear"); }

int Client::AIInput() {
  unsigned int i = 0;
  bool found = false;
  while (!found && i < 9) {
    if (BOARD[i] == 0) {
      BOARD[i] = 2; // 一下能赢的棋
      if (checkWinner() == 2) {
        found = true;
      }
      BOARD[i] = 0;

      BOARD[i] = 1; // 不下会输的棋
      if (checkWinner() == 1) {
        found = true;
      }
      BOARD[i] = 0;
    }
    i++;
  }

  if (!found) { //第三种策略
    const int next[9] = {4, 0, 2, 6, 8, 1, 3, 5, 7};
    unsigned int k = 0;
    while (!found && k < 9) {
      if (BOARD[next[k]] == 0) {
        found = true;
      }
      k++;
    }
    return next[k - 1];
  } else {
    return i - 1; // 前两中策略有效
  }
}

void Client::PlayAI() {
  clearScreen();
  showBoard();
  int winner = -1;
  // 默认玩家 1 是用户，玩家 2 是电脑
  int count = 0, turn;
  while (count < 9) {
    int pos;
    turn = count % 2 + 1;
    if (turn == 2) {
      pos = AIInput();
    } else {
      std::cout << "You turn: ";
      pos = checkInput();
    }
    BOARD[pos] = turn;
    clearScreen();
    showBoard();
    winner = checkWinner();
    if (winner != -1) {
      break;
    }
    count++;
  }

  switch (winner) {
  case 1:
    std::cout << "YOU WIN!" << std::endl;
    break;
  case 2:
    std::cout << "you lose!" << std::endl;
    break;
  case 0:
    std::cout << "Nobody win" << std::endl;
    break;
  }
  std::cout << "Game Over. Press ENTER to continue" << std::endl;
  this->Reset(); // 不重置再重新开局的话就棋盘还在
  getchar();     // 为了缓冲暂停一下，一个不够
  getchar();
}

void Client::Reset() {
  for (unsigned int i = 0; i < 9; i++) {
    this->BOARD[i] = 0;
  }
}

void Client::Close() { close(this->sock); }

int main() {
  Client cli;
  cli.Start();
  cli.ResetColor();
  return 0;
}
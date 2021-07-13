#ifdef __linux__
const int OS = 0;
#elif defined(_WIN32)
const int OS = 1;
#endif
#include "client.h"

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

bool Client::Connect() {
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("Error: Connection creation failed");
    close(sock);
    return false;
  } else {
    return true;
  }
}

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
    this->PlayAI();
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
  // 检测赢家，返回值 -1（无赢家） or 1（玩家一） or 2（玩家二）
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
  return winner;
}

void Client::clearScreen() {
  if (OS == 0)
    system("clear");
  else
    system("clr");
}

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
    return k - 1;
  } else {
    return i - 1; // 前两中策略有效
  }
}

void Client::PlayAI() {
  showBoard();
  int winner = -1;

  int count = 0, turn;
  while (count < 9) {
    int pos;
    turn = count % 2 + 1;
    if (turn == 2) {
      pos = AIInput();
    } else {
      std::cout << "Player " << turn << " : ";
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
  case -1:
    std::cout << "Nobody win" << std::endl;
    break;
  }
  std::cout << "Game Over. Take a rest" << std::endl;
}

void Client::Close() { close(this->sock); }

int main() {
  Client cli;
  cli.Start();
  return 0;
}
#ifdef __linux__
const int OS = 0;
#elif defined(_WIN32)
const int OS = 1;
#endif

#include <iostream>
#include <string.h>

using namespace std;
int BOARD[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

void showBoard() {
  // 显示棋盘以及棋子
  for (int i = 0; i < 9; i++) {
    cout << " __";
    if (BOARD[i] == 0) {
      cout << i;
    } else {
      if (BOARD[i] == 1) {
        cout << 'x';
      } else {
        cout << 'A';
      }
    }

    if ((i + 1) % 3 == 0) {
      cout << "__" << endl;
    } else {
      cout << "__ |";
    }
  }
}

int checkWinner() {
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

// 获取用户的合法输入
int checkInput() {
  int pos = -1;
  while (pos < 0 || pos > 8) {
    cin >> pos;
    if (BOARD[pos] != 0) {
      cout << "Position is not null, try again" << endl;
      pos = -1;
    }
  }
  return pos;
}

void clearScreen() {
  // 清空控制台所有内容, linux 与 windows 有所不同
  if (OS == 0)
    system("clear");
  else
    system("clr");
}

// AI 输入
int AIInput() {
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

int main() {
  // 0 表示未有子，1、2分别表示两种棋子

  bool AImodel = false;
  char command[10];
  cout << "Play with AI？yes or no" << endl;
  cin.getline(command, 10);

  if (!strcmp(command, "yes")) { // AI
    AImodel = true;
  }

  clearScreen();
  showBoard();
  int winner = -1;

  int count = 0, turn;
  while (count < 9) {
    int pos;
    turn = count % 2 + 1;
    if (AImodel && turn == 2) {
      pos = AIInput();
      //   cout << "AI: " << pos << endl;
    } else {
      cout << "Player " << turn << " : ";
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
    cout << "YOU WIN!" << endl;
    break;
  case 2:
    cout << "you lose!" << endl;
    break;
  case -1:
    cout << "Nobody win" << endl;
    break;
  }
  cout << "Game Over. Take a rest" << endl;
  return 0;
}

#ifdef __linux__
const int OS = 0;
#elif defined(_WIN32)
const int OS = 1;
#endif

#include <iostream>
using namespace std;
int BOARD[9] = {0};

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

int main() {
  // 0 表示未有子，1、2分别表示两种棋子
  clearScreen();
  showBoard();
  int winner = -1;

  int count = 0, turn;
  while (count < 9) {
    int pos;
    turn = count % 2 + 1;
    cout << "Player " << turn << " : ";
    pos = checkInput();
    BOARD[pos] = turn;
    clearScreen();
    showBoard();
    winner = checkWinner();
    if (winner != -1) {
      break;
    }
    count++;
  }
  cout << "The winner is Player : " << winner << endl;
  return 0;
}

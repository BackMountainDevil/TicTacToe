#ifdef __linux__
const int OS = 0;
#elif defined(_WIN32)
const int OS = 1;
#endif

#include <iostream>
using namespace std;

void showBoard(int board[]) {
  // 显示棋盘以及棋子
  for (int i = 0; i < 9; i++) {
    cout << " __";
    if (board[i] == 0) {
      cout << i;
    } else {
      if (board[i] == 1) {
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

int checkWinner(int board[]) {
  // 检测赢家，返回值 -1（无赢家） or 1（玩家一） or 2（玩家二）
  int winner = -1;
  for (int i = 0; i < 3; i++) {
    if (board[i] == board[i + 3] && board[i + 3] == board[i + 6] &&
        board[i] != 0) { // 竖
      winner = board[i];
      break;
    } else if (board[i * 3] == board[i * 3 + 1] &&
               board[i * 3 + 1] == board[i * 3 + 2] &&
               board[i * 3] != 0) { // 横
      winner = board[i * 3];
      break;
    }
  }
  // 对角线
  if (board[0] == board[4] && board[4] == board[8] && board[4] != 0) {
    winner = board[4];
  } else if (board[2] == board[4] && board[4] == board[6] && board[4] != 0) {
    winner = board[4];
  }
  return winner;
}

// 获取用户的合法输入
int checkInput(int board[]) {
  int pos = -1;
  while (pos < 0 || pos > 8) {
    cin >> pos;
    if (board[pos] != 0) {
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
  int board[9] = {0};
  clearScreen();
  showBoard(board);
  int winner = -1;

  int count = 0;
  while (count < 9) {
    int pos;
    cout << "Player " << count % 2 + 1 << " : ";
    pos = checkInput(board);
    board[pos] = count % 2 + 1;
    clearScreen();
    showBoard(board);
    winner = checkWinner(board);
    if (winner != -1) {
      break;
    }
    count++;
  }
  cout << "The winner is Player : " << winner << endl;
  return 0;
}

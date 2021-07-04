#include<iostream>
using namespace std;

void showBoard(int board[]){
    // 显示棋盘以及棋子    
    for(int i=0; i<9; i++){
        cout<<" __";
        if(board[i]==0){
            cout<<i;
        }else{
            if(board[i]==1){
                cout<<'x';
            }else{
                cout<<'A';
            }
        }

        if((i+1)%3==0){
            cout<<"__"<<endl;
        }else{
            cout<<"__ |";
        }
    }
}

int checkWinner(int board[]){
    // 检测赢家，返回值 -1（无赢家） or 1（玩家一） or 4（玩家二）
    int winner = -1;
    for(int i=0; i<3; i++){
        if(board[i]+board[i+3]+board[i+6] == 12 || board[i]+board[i+3]+board[i+6] == 3){   // 竖
            winner = board[i];
            break;
        }else if(board[i*3]+board[i*3+1]+board[i*3+2] ==12 || board[i*3]+board[i*3+1]+board[i*3+2] ==3){   // 横
            winner = board[i*3];
            break;
        }
    }
    // 对角线
    if(board[0]+board[4]+board[8]==3 || board[0]+board[4]+board[8]==12){
        winner = board[4];
    }else if(board[2]+board[4]+board[6]==3 || board[2]+board[4]+board[6]==12){
        winner = board[4];
    }
    return winner;
}

void checkInput(int pos, int board[])
{
    if(pos<0 || pos>8){
        cout<<"input out of range, exit"<<endl;
        exit(0);
    }else if(board[pos] != 0){
        cout<<"pos is not null, exit"<<endl;
        exit(0);
    }
}

int main(){
    // 0 表示未有子，1、4分别表示两种棋子
    int board[9] = {0}; 
    system( "clear" );
    showBoard(board);
    int winner = -1;

    int count = 0;
    while(count < 9){
        int pos;

        cout<<"Play 1 (x):";
        cin>>pos;
        checkInput(pos,board); 
        board[pos] = 1;
        system( "clear" );
        showBoard(board);
        winner = checkWinner(board);
        if(winner!=-1){
            break;
        }
        count++;
        if(count>8) break;

        cout<<"Play 2 (A):";
        cin>>pos;
        checkInput(pos,board);
        board[pos] = 4;
        system( "clear" );
        showBoard(board);
        winner = checkWinner(board);
        if(winner!=-1){
            break;
        }
        count++;
    }
    winner=winner==1?1:2;
    cout<<"The winner is Player : "<<winner<<endl;
    return 0;
}

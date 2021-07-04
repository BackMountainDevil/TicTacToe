#include<iostream>
using namespace std;

void showBoard(int board[9]){
    // 显示棋盘以及棋子    
    for(int i=0; i<9; i++){
        // cout<<"__"<<(board[i]==-1?i:(board[i]==0?'x':'A'))<<"__|";   // 120 or 65 输出不是对于字符，而是acsii
        cout<<" __";
        if(board[i]==-1){
            cout<<i;
        }else{
            if(board[i]==0){
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

int main(){
    // -1 表示未有子，0、1分别表示两种棋子
    int board[9] = {-1, 0, 1, -1, -1, -1, -1, -1, -1}; 
    showBoard(board);
    return 0;
}

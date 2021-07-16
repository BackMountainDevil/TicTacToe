# 井字棋
目前仅支持 Linux 平台。
## 如何使用
```bash
# gcc 编译， C++ 11 标准
make

# 运行服务端程序，如果是不打算联机可以不用运行服务端
./server

# 运行客户端
./client
```
使用键盘 `W/S` 进行上下选择，`回车键` 进行确认。落子时选择输入对于棋子的位置（0～8）然后回车即可。内置单机模式和联机模式，单机模式下可尝试重新连接服务端。无论是单机还是联机，均可以与 AI 对战。


## 游戏逻辑
双方轮流落子直到诞生赢家或者平手（九个位置均有棋子）

1. 初始化棋盘
2. 玩家一落子
3. 检查落子是否合法，不合法则结束游戏或者返回步骤2重新落子
4. 判断是否产生赢家或者平手，是则结束游戏，否则继续下一步
5. 切换玩家，回到步骤2

## 输赢判断
代码中用一维数组表示（board[9]）棋盘的九个位置，刚开始默认九个都是空（Value0,以 0 表示），玩家一（Player 1）落子时对应位置的值更换为 Value1，玩家二（Player 2）落子时对应位置的值更换为 Value2。

井子棋中赢家有八种获胜的排列方式：三横三竖两对角。前两者可用循环结构。

### 等值逻辑
只要求 Value0、Value1、Value2 互不相同即可，分别判断八种情况中是否有获胜者（某条直线段上的三个值都相等）。

需要注意的是， C++ 中并不支持多个连续等值比较（如 a==b==c），会涉及到隐式的赋值，应使用逻辑且（如 a==b && b==c）。

### 求和逻辑
这个要求上不是很好描述，举个例子：Value0=0，Value1=1, Value2=4。这个时候判断获胜逻辑是某条直线段上的三个值的和，和为 3 的时候玩家一胜出，和为 12 的时候玩家二胜出。一个不好的例子是（0,1,2），因为和为 3 的时候不一定是（1,1,1），也可能是（0,1,2），这样子无法和输赢建立等价关系。

## AI
1. 如果下这一步能赢，则下这一步
2. 如果玩家下这一步会赢，则下这一步堵死它
3. 如果上两条无效，则优先顺序：中心、四个角、else

# 网络通信
## 代码兼容性
在 win 用 `mingw32-make` 代替 `make` 尝试编译不通过，结合 socket 编程得知 windwos 平台的套接字编程与 Linux 有所不同，目前代码仅适配 Linux 平台。

## 通信对象
1. 服务器
  获取客户信息
2. 特定客户
  1 V 1 交换数据报

## 通信格式
### 第一版
就一个字符串，如果开头是斜杠，表明是特殊指令。如 \Q 代表获取全服在线但不在游戏中的客户编号，12 号输入 \p 23 hello world 代表向客户 23 发送 hello world，23 号就会收到  \pf 12 hello world 。但是分割字符串麻烦，过于繁琐

- \Q: 获取全服在线但不在游戏中的客户编号
- \G: 告诉服务端我游戏开始了，不再接受匹配
- \W: 告诉服务端我游戏结束了，蹲好友一起玩
- \S: 告诉服务器，给我分配一个对手
- \p: 定向转发给指定客户

比较考验各种类型之间的转换

- [C/C++ | 字符型数组char*的分割和string的分割 | strtok()函数 | find()和substr()函数 stone_fall 2019-03-18](https://blog.csdn.net/stone_fall/article/details/88640469)

# 三步走开发过程
1. 单机双人井子棋
2. AI 井子棋
3. 网络对战井子棋

网络对战有点麻烦，虽然知道是 socket 编程，用 asio 或者 rpc 也还没有入门太深，便重新抓起 [socket](https://github.com/BackMountainDevil/socket)，把 socket 重新撸了一遍，然后从中挑一个适合自己的加入井字棋中。加入这一步花了很长时间尝试怎么握手，要不要三次握手。。通信格式也是最粗糙的，目前是能正常玩耍，整完网络之后，然后想着控制台太单调了，整点 GUI，用 easyx，MFC，Qt 似乎都行，不过文档中给的案例是控制台版本，那就在控制台上整点颜色瞧瞧吧，于是又撸了 [VT100](https://github.com/BackMountainDevil/BeautyConsoleMenu).
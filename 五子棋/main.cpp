#include<stdio.h>
#include<easyx.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<math.h>

#define ROWS 15
#define COLS 15
#define GRID_SIZE 38
#define XOFFSET 213
#define YOFFSET 34

enum ChessType
{
    None = 0,
    Black = 1,
    White = -1
};

//游戏结构
struct Game
{
    //0 表示没有棋子 1表示黑棋 -1 表示白棋
    int map[ROWS][COLS];
    //游戏是否在运行
    bool isRunning;
    //定义消息变量
    ExMessage msg;

    //鼠标当前所在的下标
    int row;
    int col;

    //当前棋手
    ChessType currentChessType;

};

//游戏初始化
void init(Game* pthis, int w, int h);
//游戏绘制
void render(Game* pthis);
//游戏更新
void update(Game* pthis);

bool horiaontal(Game* pthis, int chess);
bool vertical(Game* pthis, int chess);
bool leftOblique(Game* pthis, int chess);
bool RightOblique(Game* pthis, int chess);
bool judge(Game* pthis);

int main()
{
    Game game;
    init(&game, 960, 600);

    //图片
    IMAGE img_bg;
    //使用宽字符字符串，解决字符集相关问题
    loadimage(&img_bg, L"Resource/images/bk.jpg");

    //处理鼠标操作，不断地去处理

    //游戏主循环
    while (game.isRunning)
    {
        if (judge(&game))
        {
            MessageBox(GetHWnd(), L"结束了", L"....", MB_OK);
            game.isRunning = false;
        }

        //获取消息
        if (peekmessage(&game.msg))
        {
            update(&game);
        }

        //开始批量绘图
        BeginBatchDraw();
        //清屏
        cleardevice();
        putimage(0, 0, &img_bg);

        render(&game);

        //结束双缓冲绘图
        EndBatchDraw();

    }

    return 0;
}

void init(Game* pthis, int w, int h)
{
    srand(time(NULL));
    //创建一个窗口
    initgraph(w, h, EX_SHOWCONSOLE);
    pthis->isRunning = true;
    pthis->row = -1;
    pthis->col = -1;
    pthis->currentChessType = rand() % 2 ? Black : White;
    //初始化棋盘
    memset(pthis->map, 0, sizeof(pthis->map));
}

void render(Game* pthis)
{
    //绘制棋子
    for (int i = 0; i < ROWS; i++)
    {
        for (int k = 0; k < COLS; k++)
        {
            if (pthis->map[i][k] != None)
            {
                //求每个格子左上角的坐标
                int x = k * GRID_SIZE + XOFFSET;
                int y = i * GRID_SIZE + YOFFSET;

                //绘制棋子
                if (pthis->map[i][k] == White)
                {
                    setfillcolor(WHITE);
                }
                else if (pthis->map[i][k] == Black)
                {
                    setfillcolor(BLACK);
                }

                solidcircle(x, y, 15);
            }

        }
    }

    //绘制当前鼠标所在的提示框
    printf("%d %d\n", pthis->row, pthis->col);
    if (pthis->row != -1 && pthis->col != -1)
    {
        setlinecolor(BLUE);
        int x = pthis->col * GRID_SIZE + XOFFSET;
        int y = pthis->row * GRID_SIZE + YOFFSET;
        circle(x, y, 17);
    }
}

void update(Game* pthis)
{
    //鼠标移动
    if (pthis->msg.message == WM_MOUSEMOVE)
    {
        for (int i = 0; i < ROWS; i++)
        {
            for (int k = 0; k < COLS; k++)
            {
                int cx = k * GRID_SIZE + XOFFSET;
                int cy = i * GRID_SIZE + YOFFSET;

                if (abs(pthis->msg.x - cx) < GRID_SIZE / 2 && abs(pthis->msg.y - cy) < GRID_SIZE / 2)
                {
                    pthis->row = i;
                    pthis->col = k;
                    goto END_LOOP;
                }
            }
        }
    END_LOOP:;

    }
    else if (pthis->msg.message == WM_LBUTTONDOWN &&    //鼠标左键点击
        pthis->row != -1 && pthis->col != -1 &&            //点击了合法的位置
        pthis->map[pthis->row][pthis->col] == None)        //当前位置没有棋子
    {
        pthis->map[pthis->row][pthis->col] = pthis->currentChessType;

        //切换棋手
        pthis->currentChessType = (ChessType)-pthis->currentChessType;
    }
}

bool judge(Game* pthis)
{
    int who = -pthis->currentChessType;
    if (horiaontal(pthis, who))
        return true;

    if (vertical(pthis, who))
        return true;

    if (leftOblique(pthis, who))
        return true;

    if (RightOblique(pthis, who))
        return true;
    return false;
}

bool horiaontal(Game* pthis, int chess)
{
    //横的
    for (int c = pthis->col - 4; c <= pthis->col; c++)
    {
        int counter = 0;
        for (int n = 0; n < 5; n++)
        {
            if (c + n >= 0 && c + n < COLS && pthis->map[pthis->row][c + n] == chess)
            {
                counter++;
            }
        }
        if (counter == 5)
            return true;
    }
    return false;
}

bool vertical(Game* pthis, int chess)
{
    //垂直的
    for (int r = pthis->row - 4; r <= pthis->row; r++)
    {
        int counter = 0;
        for (int n = 0; n < 5; n++)
        {
            if (r + n >= 0 && r + n < ROWS && pthis->map[r + n][pthis->col] == chess)
            {
                counter++;
            }
        }
        if (counter == 5)
            return true;
    }
    return false;
}

bool leftOblique(Game* pthis, int chess)
{
    //左斜的
    for (int offset = 0; offset < 4; offset++)
    {
        int start_r = pthis->row - offset;
        int start_c = pthis->col - offset;
        int counter = 0;
        for (int n = 0; n < 5; n++)
        {
            if (start_r + n >= 0 && start_r + n < ROWS && start_c + n >= 0 && start_c + n < COLS && pthis->map[start_r + n][start_c + n] == chess)
            {
                counter++;
            }
        }
        if (counter == 5)
            return true;
    }
    return false;
}

bool RightOblique(Game* pthis, int chess)
{
    //右斜的
    for (int offset = 0; offset < 4; offset++)
    {
        int start_r = pthis->row + offset;
        int start_c = pthis->col - offset;
        int counter = 0;
        for (int n = 0; n < 5; n++)
        {
            if (start_r - n >= 0 && start_r - n < ROWS && start_c + n >= 0 && start_c + n < COLS && pthis->map[start_r - n][start_c + n] == chess)
            {
                counter++;
            }
        }
        if (counter == 5)
            return true;
    }
    return false;
}
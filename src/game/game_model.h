#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <vector>

// ------------ 全局变量 ------------ //
// 最大行和列数
const int MAX_ROW = 15;
const int MAX_COL = 20;
// 游戏可玩图片数量
const int MAX_ICON = 20;
// 游戏状态
enum GameStatus
{
    PLAYING,
    PAUSE,
    WIN,
    OVER
};
// 游戏难度，不同的方块数
const int kBasicNum = MAX_ROW * MAX_COL * 0.3;
const int kMediumNum = MAX_ROW * MAX_COL * 0.7;
const int kHardNum = MAX_ROW * MAX_COL;

const int MAXHINTS = 5;

enum GameLevel
{
    BASIC,
    MEDIUM,
    HARD
};

// 用于绘制线段的连接点
struct PaintPoint
{
    PaintPoint(int _x, int _y) : x(_x), y (_y) {}
    int x;
    int y;
};

// -------------------------------- //

#endif // GAMEMODEL_H

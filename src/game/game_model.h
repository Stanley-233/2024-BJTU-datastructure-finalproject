#ifndef GAMEMODEL_H
#define GAMEMODEL_H

#include <vector>

// ------------ 全局变量 ------------ //
// 最大行和列数
constexpr int MAX_ROW = 15;
constexpr int MAX_COL = 20;
// 游戏可玩图片数量
constexpr int MAX_ICON = 20;

// 游戏状态
enum GameStatus {
    PLAYING,
    PAUSE,
    WIN,
    OVER
};

// 游戏难度，不同的方块数
constexpr int kBasicNum = MAX_ROW * MAX_COL * 0.2;
constexpr int kMediumNum = MAX_ROW * MAX_COL * 0.5;
constexpr int kHardNum = MAX_ROW * MAX_COL * 0.7;

constexpr int MAXHINTS = 5;

enum GameLevel {
    BASIC,
    MEDIUM,
    HARD
};

// 用于绘制线段的连接点
struct PaintPoint {
    PaintPoint(int _x, int _y) :
        x(_x), y(_y) {
    }
    int x;
    int y;
};

// -------------------------------- //

class GameModel final {
public:
    GameModel();
    ~GameModel();

    void startGame(); // 开始游戏
    void startGame(GameLevel level);
    int *getGameMap() const; // 获得地图

    bool linkTwoTiles(int srcX, int srcY, int dstX, int dstY); // 连接起点和终点方块，连接是否成功
    bool isFrozen(); // 判断是否已经成为了僵局
    bool isWin(); // 检查游戏是否结束
    int *getHint() const; // 获得提示
    std::vector<PaintPoint> paintPoints; // 用于绘制的点

    // 游戏状态和难度
    GameStatus gameStatus;
    GameLevel gameLevel;

    int getScore() const;
    void setScore(int score);

    int getTimes() const;
    void setTimes(int times);

    int getHintsLast() const;
    void setHintsLast(int hintsLast);

    //打乱剩余方块
    void reset() const;

private:
    // 游戏地图，存储方块，0表示消失，1-其他数字表示图片标号
    int *gameMap;

    int score;

    int hintsLast;
    //记录消去方块数
    int times = 0;

    // 游戏提示，存储2个点
    int *hintArray;

    // 判断起点到终点的方块是否可以连接消除
    bool isCanLink(int srcX, int srcY, int dstX, int dstY);

    bool canLinkDirectly(int srcX, int srcY, int dstX, int dstY);
    bool canLinkWithOneCorner(int srcX, int srcY, int dstX, int dstY);
    bool canLinkWithTwoCorner(int srcX, int srcY, int dstX, int dstY);

    // 提示模式还是连接模式判断
    bool isFrozenMode;

};


#endif // GAMEMODEL_H

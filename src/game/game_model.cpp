#include <cstdlib>
#include <ctime>
#include <algorithm>
#include "game_model.h"

using namespace std;

// 游戏逻辑模型，与界面分离
GameModel::GameModel() :
    gameStatus(PLAYING),
    gameLevel(BASIC) {
}

GameModel::~GameModel() {
    if (gameMap) {
        delete[] gameMap;
        delete[] hintArray;
        gameMap = nullptr;
    }
}

void GameModel::startGame(GameLevel level) {

    score = 0;

    hintsLast = MAXHINTS;

    gameMap = new int[MAX_ROW * MAX_COL];
    for (int i = 0; i < MAX_ROW * MAX_COL; i++)
        gameMap[i] = 0;

    hintArray = new int[4];
    for (int i = 0; i < 4; i++)
        hintArray[i] = -1;

    gameStatus = PLAYING;

    gameLevel = level;

    int gameLevelNum;
    switch (gameLevel) {
        case BASIC:
            gameLevelNum = kBasicNum;
            break;
        case MEDIUM:
            gameLevelNum = kMediumNum;
            break;
        case HARD:
            gameLevelNum = kHardNum;
    }
    // 填充方块标号
    int iconID = 0;
    for (int i = 0; i < gameLevelNum; i += 2) {
        // 每次填充连着的两个，图片用尽了就循环
        gameMap[i] = iconID % MAX_ICON + 1;
        gameMap[i + 1] = iconID % MAX_ICON + 1;
        iconID++;
    }
    // 打乱方块
    reset();
    // 初始化判断模式
    isFrozenMode = false;
    // 初始化绘制点
    paintPoints.clear();
}

// 重载
void GameModel::startGame() {
    startGame(gameLevel);
}

int *GameModel::getGameMap() const {
    return gameMap;
}

bool GameModel::isFrozen() {
    // 暴力法，所有方块两两判断是否可以连接
    // 每次消除后做一次判断
    // 其实在这个过程中记录提示

    for (int i = 0; i < MAX_ROW * MAX_COL - 1; i++)
        for (int j = i + 1; j < MAX_ROW * MAX_COL; j++) {
            int srcX = i % MAX_COL;
            int srcY = i / MAX_COL;
            int dstX = j % MAX_COL;
            int dstY = j / MAX_COL;

            // 只要能找到可以连接的就不为僵局
            isFrozenMode = true;
            if (isCanLink(srcX, srcY, dstX, dstY)) {
                // 记录第一个可以连接的hint
                hintArray[0] = srcX;
                hintArray[1] = srcY;
                hintArray[2] = dstX;
                hintArray[3] = dstY;

                isFrozenMode = false;

                return false;
            }
        }
    isFrozenMode = false;

    return true;
}

bool GameModel::isWin() {
    for (int i = 0; i < MAX_ROW * MAX_COL; i++) {
        if (gameMap[i])
            return false;
    }
    //更新游戏状态
    gameStatus = WIN;
    return true;
}

int *GameModel::getHint() const {
    return hintArray;
}

int GameModel::getScore() const {
    return score;
}

void GameModel::setScore(const int score) {
    this->score = score;
}

int GameModel::getTimes() const {
    return times;
}

void GameModel::setTimes(int times) {
    this->times = times;
}

int GameModel::getHintsLast() const {
    return hintsLast;
}

void GameModel::setHintsLast(int hintsLast) {
    this->hintsLast = hintsLast;
}

void GameModel::reset() const {
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = 0; i < MAX_ROW * MAX_COL; i++) {
        int randomID = rand() % (MAX_ROW * MAX_COL);
        std::swap(gameMap[i], gameMap[randomID]);
    }
}

// 最重要的判断连接算法
bool GameModel::canLinkDirectly(int srcX, int srcY, int dstX, int dstY) {
    // 竖线
    if (srcX == dstX) {
        if (srcY > dstY)
            std::swap(srcY, dstY);
        for (int y = srcY + 1; y < dstY; y++)
            if (gameMap[MAX_COL * y + srcX])
                return false;
        if (!isFrozenMode) {
            // 记录点和路线
            PaintPoint p1(srcX, srcY), p2(dstX, dstY);
            paintPoints.clear();
            paintPoints.push_back(p1);
            paintPoints.push_back(p2);
        }
        return true;
    }

    // 横线
    if (srcY == dstY) {
        if (srcX > dstX)
            std::swap(srcX, dstX);
        for (int x = srcX + 1; x < dstX; x++)
            if (gameMap[MAX_COL * srcY + x])
                return false;
        if (!isFrozenMode) {
            PaintPoint p1(srcX, srcY), p2(dstX, dstY);
            paintPoints.clear();
            paintPoints.push_back(p1);
            paintPoints.push_back(p2);
        }
        return true;
    }
    return false;
}

bool GameModel::canLinkWithOneCorner(int srcX, int srcY, int dstX, int dstY) {
    //Todo: 待重构
    if (srcX > dstX) {
        // 统一化，方便后续处理
        std::swap(srcX, dstX);
        std::swap(srcY, dstY);
    }
    // 先确定拐点，再确定直连线路,2种情况，4个点，每种情况逐个试，所以多个if顺序执行
    if (dstY > srcY) {
        if (gameMap[srcY * MAX_COL + dstX] == 0) {
            // 右上角
            if (canLinkDirectly(srcX, srcY, dstX, srcY) && canLinkDirectly(dstX, srcY, dstX, dstY)) {
                // 只有连接模式才记录点
                if (!isFrozenMode) {
                    PaintPoint p1(srcX, srcY), p2(dstX, srcY), p3(dstX, dstY);
                    paintPoints.clear();
                    paintPoints.push_back(p1);
                    paintPoints.push_back(p2);
                    paintPoints.push_back(p3);
                }
                return true;
            }
        }
        if (gameMap[dstY * MAX_COL + srcX] == 0) {
            // 左下角
            if (canLinkDirectly(srcX, srcY, srcX, dstY) && canLinkDirectly(srcX, dstY, dstX, dstY)) {
                if (!isFrozenMode) {
                    PaintPoint p1(srcX, srcY), p2(srcX, dstY), p3(dstX, dstY);
                    paintPoints.clear();
                    paintPoints.push_back(p1);
                    paintPoints.push_back(p2);
                    paintPoints.push_back(p3);
                }
                return true;
            }
        }
    } else {
        if (gameMap[dstY * MAX_COL + srcX] == 0) {
            // 左上角
            if (canLinkDirectly(srcX, srcY, srcX, dstY) && canLinkDirectly(srcX, dstY, dstX, dstY)) {
                if (!isFrozenMode) {
                    PaintPoint p1(srcX, srcY), p2(srcX, dstY), p3(dstX, dstY);
                    paintPoints.clear();
                    paintPoints.push_back(p1);
                    paintPoints.push_back(p2);
                    paintPoints.push_back(p3);
                }
                return true;
            }
        }
        if (gameMap[srcY * MAX_COL + dstX] == 0) {
            // 右下角
            if (canLinkDirectly(srcX, srcY, dstX, srcY) && canLinkDirectly(dstX, srcY, dstX, dstY)) {
                if (!isFrozenMode) {
                    PaintPoint p1(srcX, srcY), p2(dstX, srcY), p3(dstX, dstY);
                    paintPoints.clear();
                    paintPoints.push_back(p1);
                    paintPoints.push_back(p2);
                    paintPoints.push_back(p3);
                }
                return true;
            }
        }
    }
    return false;
}

bool GameModel::canLinkWithTwoCorner(int srcX, int srcY, int dstX, int dstY)
{
    //内部用到的方便小函数，lambda实现
    auto getSmaller = [](const int a, const int b){return a > b ? b : a;};
    auto getBigger = [](const int a, const int b){return a > b ? a : b;};
    //判断是否有平行于y轴的空线段
    for(int y = 0; y < MAX_ROW; ++y){
        if(y != srcY && y != dstY){
            bool linkable = true;
            for(int x = getSmaller(srcX, dstX); x <= getBigger(srcX, dstX); x++)
                if(gameMap[y * MAX_COL + x] != 0){
                    linkable = false;
                    break;
                }
            //如果存在空线段，是否可以垂直链接其首尾
            if(linkable){
                if(canLinkDirectly(srcX, srcY, srcX, y) && canLinkDirectly(dstX, dstY, dstX, y) == true){
                    //将绘制连线的顶点依次push到队列末尾
                    if(!isFrozenMode){
                        paintPoints.clear();
                        paintPoints.push_back(PaintPoint(srcX, srcY));
                        paintPoints.push_back(PaintPoint(srcX, y));
                        paintPoints.push_back(PaintPoint(dstX, y));
                        paintPoints.push_back(PaintPoint(dstX, dstY));
                    }
                    return true;
                }
            }
        }
    }
    //x宇称后同理
    for(int x = 0; x < MAX_COL; ++x){
        if(x != srcX && x != dstX){
            bool linkable = true;
            for(int y = getSmaller(srcY, dstY); y <= getBigger(srcY, dstY); y++)
                if(gameMap[y * MAX_COL + x] != 0){
                    linkable = false;
                    break;
                }
            if(linkable){
                if(canLinkDirectly(srcX, srcY, x, srcY) && canLinkDirectly(dstX, dstY, x, dstY) == true){
                    if(!isFrozenMode){
                        paintPoints.clear();
                        paintPoints.push_back(PaintPoint(srcX, srcY));
                        paintPoints.push_back(PaintPoint(x, srcY));
                        paintPoints.push_back(PaintPoint(x, dstY));
                        paintPoints.push_back(PaintPoint(dstX, dstY));
                    }
                    return true;
                }
            }
        }
    }
    //外边缘连接实现
    bool linkable = true;
    for(int x = 0; x < srcX; ++x){
        if(gameMap[srcY * MAX_COL + x] != 0)
            linkable = false;
    }
    for(int x = 0; x < dstX; ++x){
        if(gameMap[dstY * MAX_COL + x] != 0)
            linkable = false;
    }
    if(linkable){
        if(!isFrozenMode){
            paintPoints.clear();
            paintPoints.push_back(PaintPoint(srcX, srcY));
            paintPoints.push_back(PaintPoint(-1, srcY));
            paintPoints.push_back(PaintPoint(-1, dstY));
            paintPoints.push_back(PaintPoint(dstX, dstY));
        }
        return true;
    }

    linkable = true;
    for(int x = MAX_COL - 1; x > srcX; --x){
        if(gameMap[srcY * MAX_COL + x] != 0)
            linkable = false;
    }
    for(int x = MAX_COL - 1; x > dstX; --x){
        if(gameMap[dstY * MAX_COL + x] != 0)
            linkable = false;
    }
    if(linkable){
        if(!isFrozenMode){
            paintPoints.clear();
            paintPoints.push_back(PaintPoint(srcX, srcY));
            paintPoints.push_back(PaintPoint(MAX_COL, srcY));
            paintPoints.push_back(PaintPoint(MAX_COL, dstY));
            paintPoints.push_back(PaintPoint(dstX, dstY));
        }
        return true;
    }
    linkable = true;
    for(int y = 0; y < srcY; ++y){
        if(gameMap[y * MAX_COL + srcX] != 0)
            linkable = false;
    }
    for(int y = 0; y < dstY; ++y){
        if(gameMap[y * MAX_COL + dstX] != 0)
            linkable = false;
    }
    if(linkable){
        if(!isFrozenMode){
            paintPoints.clear();
            paintPoints.push_back(PaintPoint(srcX, srcY));
            paintPoints.push_back(PaintPoint(srcX, -1));
            paintPoints.push_back(PaintPoint(dstX, -1));
            paintPoints.push_back(PaintPoint(dstX, dstY));
        }
        return true;
    }

    linkable = true;
    for(int y = MAX_ROW - 1; y > srcY; --y){
        if(gameMap[y * MAX_COL + srcX] != 0)
            linkable = false;
    }
    for(int y = MAX_ROW - 1; y > dstY; --y){
        if(gameMap[y * MAX_COL + dstX] != 0)
            linkable = false;
    }
    if(linkable){
        if(!isFrozenMode){
            paintPoints.clear();
            paintPoints.push_back(PaintPoint(srcX, srcY));
            paintPoints.push_back(PaintPoint(srcX, MAX_ROW));
            paintPoints.push_back(PaintPoint(dstX, MAX_ROW));
            paintPoints.push_back(PaintPoint(dstX, dstY));
        }
        return true;
    }
    //都没有找到合适的空行，返回无法链接的判定
    return false;
}

bool GameModel::isCanLink(int srcX, int srcY, int dstX, int dstY) {
    // 首先判断点击的两个方块不是同一个不是空，且方块相同
    // 判断方块是否可以连，可用于实际的连接消除和提示消除
    // x表示横向索引，y表示纵向索引，从0开始
    // 分3种情况往下找，每一种都可以用前面简单情况组合找到一种情况可以连通就返回true，并选用这种连接情况
    if (gameMap[srcY * MAX_COL + srcX] == 0 || gameMap[dstY * MAX_COL + dstX] == 0)
        return false;
    if (srcX == dstX && srcY == dstY)
        return false;
    if (gameMap[MAX_COL * srcY + srcX] != gameMap[MAX_COL * dstY + dstX])
        return false;
    // 情况1：横向或者竖向可以直线连通
    if (canLinkDirectly(srcX, srcY, dstX, dstY))
        return true;
    // 情况2：一次拐弯可以连通
    if (canLinkWithOneCorner(srcX, srcY, dstX, dstY))
        return true;
    // 情况3：两次拐弯可以连通
    if (canLinkWithTwoCorner(srcX, srcY, dstX, dstY))
        return true;
    return false;
}

// 点击方块进行连接操作
bool GameModel::linkTwoTiles(int srcX, int srcY, int dstX, int dstY) {
    // 成功连接就返回true否则false用于GUI里面判断
    if (isCanLink(srcX, srcY, dstX, dstY)) {
        // 值重置
        gameMap[MAX_COL * srcY + srcX] = 0;
        gameMap[MAX_COL * dstY + dstX] = 0;
        return true;
    }
    return false;
}

void GameModel::startGameWithSeed(qint64 seed) {
    // TODO
}

void GameModel::resetWithSeed(qint64 seed) const {
    // TODO
}

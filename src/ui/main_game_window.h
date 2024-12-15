#ifndef MAIN_GAME_WINDOW_H
#define MAIN_GAME_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

#include "../network/network_handler.h"
#include "../game/game_model.h"

namespace Ui {
    class MainGameWindow;
    struct IconPutton;
}

struct IconButton : QPushButton {
    IconButton(QWidget *parent = Q_NULLPTR) :
        QPushButton(parent),
        xID(-1),
        yID(-1) {
    }

    int xID; // x 坐标
    int yID; // y 坐标
};

class MainGameWindow final : public QMainWindow {
    Q_OBJECT

    // GUI 变量
    const int kIconSize = 36;
    const int kTopMargin = 100;
    const int kLeftMargin = 50;

    const QString kIconReleasedStyle = "";
    const QString kIconClickedStyle = "background-color: rgba(255, 255, 12, 161)";
    const QString kIconHintStyle = "background-color: rgba(255, 0, 0, 255)";

    const int kGameTimeTotal = 5 * 60 * 1000; // 总时间
    const int kGameTimerInterval = 300;
    const int kLinkTimerDelay = 700;

public:
    explicit MainGameWindow(QWidget *parent = nullptr);
    ~MainGameWindow() override;
    bool eventFilter(QObject *watched, QEvent *event) override; // 事件过滤

private slots:
    void onLoginMessage(bool mode, QString name, QString password);
    void on_muteBtn_clicked();
    void onIconButtonPressed();
    void handleLinkEffect();
    void gameTimerEvent();
    void on_pauseBtn_clicked();
    void on_resetBtn_clicked();
    void on_again(GameLevel mode);
    void on_exit();
signals:
    void login_dialog_message(bool isSuccess, bool mode, QString info);

private:
    // GUI
    Ui::MainGameWindow *ui;
    NetworkHandler networkHandler;
    QAudioOutput audioOutput;
    QMediaPlayer player;
    QTimer *gameTimer; // 游戏计时器
    GameModel *game;
    IconButton *imageButton[MAX_ROW * MAX_COL]; // 图片button数组
    IconButton *preIcon, *curIcon; // 记录点击的icon
    // Logic
    GameLevel curLevel;
    bool isLinking; // 维持一个连接状态的标志
    bool isReallylinked;

    void initGame(GameLevel);
    inline void allFunBtnEnable(bool);
    inline void allBlocksEnable(bool state);
    void init_imageBtn(bool mode);
    void gameOver(bool mode);
};

#endif // MAIN_GAME_WINDOW_H

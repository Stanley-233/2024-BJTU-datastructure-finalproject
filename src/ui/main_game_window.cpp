#include "main_game_window.h"
#
#include "login_dialog.h"
#include "ui_main_game_window.h"

MainGameWindow::MainGameWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainGameWindow)
    , networkHandler(NetworkHandler(this))
    , player(QMediaPlayer(this))
    , audioOutput(QAudioOutput(this)) {
    ui->setupUi(this);
    this->setWindowTitle("巡旅联觉 - Traveller's Linkage");

    // // 登录
    // connect(&networkHandler, &NetworkHandler::successfulLogin, this, [this] {
    //     emit login_dialog_message(true, true, "登录成功！");
    // });
    // connect(&networkHandler, &NetworkHandler::wrongPassword, this, [this] {
    //     emit login_dialog_message(false, true, "登陆失败：密码错误！");
    // });
    // connect(&networkHandler, &NetworkHandler::noUser, this, [this] {
    //     emit login_dialog_message(false, true, "登陆失败：用户未注册！");
    // });
    // connect(&networkHandler, &NetworkHandler::serverError, this, [this] {
    //     emit login_dialog_message(false, true, "登陆失败：服务器错误！");
    // });
    // connect(&networkHandler, &NetworkHandler::successfulRegister, this, [this] {
    //     emit login_dialog_message(true, false, "注册成功！");
    // });
    // connect(&networkHandler, &NetworkHandler::alreadyRegistered, this, [this] {
    //     emit login_dialog_message(false, false, "注册失败：已存在用户！");;
    // });
    // LoginDialog lg(this);
    // connect(&lg, &LoginDialog::sendingUserMsg, this, &MainGameWindow::onLoginMessage);
    // connect(this, &MainGameWindow::login_dialog_message, &lg, &LoginDialog::on_DialogState);
    // lg.exec();

    // BGM
    player.setAudioOutput(&audioOutput);
    player.setSource(QUrl("qrc:/res/sound/bgm.mp3"));
    audioOutput.setVolume(100);
    player.setLoops(-1); // 无限循环
    player.play();

    initGame(BASIC);
}

MainGameWindow::~MainGameWindow() {
    delete ui;
}

void MainGameWindow::onLoginMessage(bool mode, QString name, QString password) {
    // True: login, false: register
    if (mode) {
        networkHandler.login(name, password);
    } else {
        networkHandler.registerUser(name, password);
    }
}

void MainGameWindow::on_muteBtn_clicked() {
    if (player.isPlaying()) {
        // qDebug() << "Mute";
        player.pause();
        ui->muteBtn->setIcon(QIcon(":/res/image/close.png"));
    } else {
        // qDebug() << "Out of mute";
        player.play();
        ui->muteBtn->setIcon(QIcon(":/res/image/open.png"));
    }
}

void MainGameWindow::initGame(GameLevel level) {
    // 启动游戏
    game = new GameModel;
    game->startGame(level);

    curLevel = level;

    ui->scoreLab->setText(QString::number(game->getScore()));

    ui->hintsLastLab->setText(QString::number(game->getHintsLast()));

    allFunBtnEnable(true);

    // 根据imagemap初始化imagebutton
    init_imageBtn(true);

    // 进度条
    ui->timeBar->setMaximum(kGameTimeTotal);
    ui->timeBar->setMinimum(0);
    ui->timeBar->setValue(kGameTimeTotal);

    // 游戏计时器
    gameTimer = new QTimer(this);
    connect(gameTimer, SIGNAL(timeout()), this, SLOT(gameTimerEvent()));
    //设置发出timeout()信号的间隔
    gameTimer->start(kGameTimerInterval);

    // 连接状态值
    preIcon = nullptr;
    isLinking = false;
    isReallylinked = false;
}

void MainGameWindow::allFunBtnEnable(const bool state) {
    ui->hintBtn->setEnabled(state);
    ui->robot_btn->setEnabled(state);
    ui->resetBtn->setEnabled(state);
    ui->pauseBtn->setEnabled(state);
}

void MainGameWindow::init_imageBtn(bool mode) {
    for (int i = 0; i < MAX_ROW * MAX_COL; i++) {
        if (mode)
            imageButton[i] = new IconButton(this);
        imageButton[i]->setGeometry(kLeftMargin + (i % MAX_COL) * kIconSize, kTopMargin + (i / MAX_COL) * kIconSize,
                                    kIconSize, kIconSize);
        // 设置索引
        imageButton[i]->xID = i % MAX_COL;
        imageButton[i]->yID = i / MAX_COL;

        imageButton[i]->show();

        if (game->getGameMap()[i]) {
            // 有方块就设置图片
            QPixmap iconPix;
            QString fileString;
            fileString = QString(":/res/image/%1.png").arg(game->getGameMap()[i]);
            iconPix.load(fileString);
            QIcon icon(iconPix);
            imageButton[i]->setIcon(icon);
            imageButton[i]->setIconSize(QSize(kIconSize, kIconSize));

            // 添加按下的信号槽
            connect(imageButton[i], SIGNAL(pressed()), this, SLOT(onIconButtonPressed()));
        } else
            imageButton[i]->hide();
    }
}


void MainGameWindow::onIconButtonPressed() {
    // 如果当前有方块在连接，不能点击方块
    if (isLinking) {
        // 播放音效
        // QSound::play(":/res/sound/release.wav");
        return;
    }
    // 记录当前点击的icon
    curIcon = dynamic_cast<IconButton *>(sender());
    if (!preIcon) {
        // 播放音效
        // QSound::play(":/res/sound/select.wav");
        curIcon->setStyleSheet(kIconClickedStyle);
        preIcon = curIcon;
    } else {
        if (curIcon != preIcon) {
            // 如果不是同一个button就都标记,尝试连接
            curIcon->setStyleSheet(kIconClickedStyle);
            if (game->linkTwoTiles(preIcon->xID, preIcon->yID, curIcon->xID, curIcon->yID)) {
                // 锁住当前状态
                isLinking = true;
                // 播放音效
                // QSound::play(":/res/sound/pair.wav");
                //重绘, 画出连接线
                update();
                // 延迟后实现连接效果
                QTimer::singleShot(kLinkTimerDelay, this, SLOT(handleLinkEffect()));
                //每连接五次奖励一次提示
                game->setTimes(game->getTimes() + 1);
                if (game->getTimes() % 5 == 0 && game->getTimes() != 0)
                    game->setHintsLast(game->getHintsLast() + 1);
                ui->hintsLastLab->setText(QString::number(game->getHintsLast()));
                //每次消除一对分数加三
                game->setScore(game->getScore() + 3);
                ui->scoreLab->setText(QString::number(game->getScore()));
                isReallylinked = false;
                // TODO:检查是否胜利
                // if (game->isWin()) gameOver(1);
            } else {
                // 播放音效
                // QSound::play(":/res/sound/release.wav");
                // 消除失败，恢复
                preIcon->setStyleSheet(kIconReleasedStyle);
                curIcon->setStyleSheet(kIconReleasedStyle);
                preIcon = nullptr;
                curIcon = nullptr;
            }
        } else if (curIcon == preIcon) {
            // 播放音效
            // QSound::play(":/res/sound/release.wav");
            preIcon->setStyleSheet(kIconReleasedStyle);
            curIcon->setStyleSheet(kIconReleasedStyle);
            preIcon = nullptr;
            curIcon = nullptr;
        }
    }
}

void MainGameWindow::handleLinkEffect() {
    game->paintPoints.clear();
    preIcon->hide();
    curIcon->hide();
    preIcon = nullptr;
    curIcon = nullptr;
    // 重绘
    update();
    // 恢复状态
    isLinking = false;
}

void MainGameWindow::gameTimerEvent() {
    // 进度条计时效果
    if (ui->timeBar->value() <= 0) {
        gameTimer->stop();
        // TODO: Game Over
        // gameOver(false);
    } else {
        ui->timeBar->setValue(ui->timeBar->value() - 10 * kGameTimerInterval);
    }
}

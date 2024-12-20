#include "main_game_window.h"

#include <QMessageBox>

#include "over_dialog.h"
#include <QPainter>
#include "login_dialog.h"
#include "ui_main_game_window.h"

MainGameWindow::MainGameWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainGameWindow)
    , networkHandler(NetworkHandler(this))
    , player(QMediaPlayer(this))
    , audioOutput(QAudioOutput(this)) 
    {

    preIcon = nullptr;
    curIcon = nullptr;

    ui->setupUi(this);
    ui->centralWidget->installEventFilter(this);
    this->setWindowTitle("巡旅联觉 - Traveller's Linkage");

    // Menu
    connect(ui->actionBasic, &QAction::triggered, this, &MainGameWindow::createGameWithLevel);
    connect(ui->actionMedium, &QAction::triggered, this, &MainGameWindow::createGameWithLevel);
    connect(ui->actionHard, &QAction::triggered, this, &MainGameWindow::createGameWithLevel);

    connect(ui->actionIntro, &QAction::triggered, this, &MainGameWindow::informationDisplay);
    connect(ui->actionVers, &QAction::triggered, this, &MainGameWindow::informationDisplay);
    connect(ui->actionGroup, &QAction::triggered, this, &MainGameWindow::informationDisplay);

    // 登录
    connect(&networkHandler, &NetworkHandler::successfulLogin, this, [this] {
        emit login_dialog_message(true, true, "登录成功！");
    });
    connect(&networkHandler, &NetworkHandler::wrongPassword, this, [this] {
        emit login_dialog_message(false, true, "登陆失败：密码错误！");
    });
    connect(&networkHandler, &NetworkHandler::noUser, this, [this] {
        emit login_dialog_message(false, true, "登陆失败：用户未注册！");
    });
    connect(&networkHandler, &NetworkHandler::serverError, this, [this] {
        emit login_dialog_message(false, true, "登陆失败：服务器错误！");
    });
    connect(&networkHandler, &NetworkHandler::successfulRegister, this, [this] {
        emit login_dialog_message(true, false, "注册成功！");
    });
    connect(&networkHandler, &NetworkHandler::alreadyRegistered, this, [this] {
        emit login_dialog_message(false, false, "注册失败：已存在用户！");;
    });
    LoginDialog lg(this);
    connect(&lg, &LoginDialog::sendingUserMsg, this, &MainGameWindow::onLoginMessage);
    connect(this, &MainGameWindow::login_dialog_message, &lg, &LoginDialog::on_DialogState);
    lg.exec();

    // Sound
    release->setSource(QUrl::fromLocalFile(":/res/sound/release.wav"));
    pair->setSource(QUrl::fromLocalFile(":/res/sound/pair.wav"));
    select->setSource(QUrl::fromLocalFile(":/res/sound/select.wav"));
    nolink->setSource(QUrl::fromLocalFile(":/res/sound/nolink.wav"));

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
    connect(gameTimer, &QTimer::timeout, this, &MainGameWindow::gameTimerEvent);
    //设置发出timeout()信号的间隔
    gameTimer->start(kGameTimerInterval);

    // 连接状态值
    preIcon = nullptr;
    isLinking = false;
    isReallyLinked = false;

    // 设置难度加时
    switch (level) {
        case BASIC:
            kBonusTime = 100;
        case MEDIUM:
            kBonusTime = 80;
        case HARD:
            kBonusTime = 70;
    }
}

void MainGameWindow::allFunBtnEnable(const bool state) {
    ui->hintBtn->setEnabled(state);
    ui->robot_btn->setEnabled(state);
    ui->resetBtn->setEnabled(state);
    ui->pauseBtn->setEnabled(state);
}

void MainGameWindow::allBlocksEnable(bool state) {
    for (const auto &i: imageButton) {
        i->setEnabled(state);
    }
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
            connect(imageButton[i], &IconButton::pressed, this, &MainGameWindow::onIconButtonPressed);
        } else
            imageButton[i]->hide();
    }
}

void MainGameWindow::onIconButtonPressed() {
    // 如果当前有方块在连接，不能点击方块
    if (isLinking) {
        // 播放音效
        release->play();
        return;
    }
    // 记录当前点击的icon
    curIcon = dynamic_cast<IconButton *>(sender());
    if (!preIcon) {
        // 播放音效
        select->play();
        curIcon->setStyleSheet(kIconClickedStyle);
        preIcon = curIcon;
    } else {
        if (curIcon != preIcon) {
            // 如果不是同一个button就都标记,尝试连接
            curIcon->setStyleSheet(kIconClickedStyle);
            if (game->linkTwoTiles(preIcon->xID, preIcon->yID, curIcon->xID, curIcon->yID)) {
                // 锁住当前状态
                isLinking = true;
                // 播放成功链接音效
                pair->play();
                //重绘, 画出连接线
                update();
                // 延迟后实现连接效果
                QTimer::singleShot(kLinkTimerDelay, this, &MainGameWindow::handleLinkEffect);
                //每连接五次奖励一次提示
                game->setTimes(game->getTimes() + 1);
                if (game->getTimes() % 5 == 0 && game->getTimes() != 0)
                    game->setHintsLast(game->getHintsLast() + 1);
                ui->hintsLastLab->setText(QString::number(game->getHintsLast()));
                //每次消除一对分数加三
                game->setScore(game->getScore() + 3);
                ui->scoreLab->setText(QString::number(game->getScore()));
                isReallyLinked = false;
                if (game->isWin())
                    gameOver(true);
                // 加时奖励
                auto bonusedTime = ui->timeBar->value() + kBonusTime * kGameTimerInterval;
                if (bonusedTime >= kGameTimeTotal) {
                    ui->timeBar->setValue(kGameTimeTotal);
                } else {
                    ui->timeBar->setValue(bonusedTime);
                }
            } else {
                // 播放音效
                nolink->play();
                // 消除失败，恢复
                preIcon->setStyleSheet(kIconReleasedStyle);
                curIcon->setStyleSheet(kIconReleasedStyle);
                preIcon = nullptr;
                curIcon = nullptr;
            }
        } else if (curIcon == preIcon) {
            // 播放音效
            release->play();
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
        gameOver(false);
    } else {
        ui->timeBar->setValue(ui->timeBar->value() - 10 * kGameTimerInterval);
    }
}

void MainGameWindow::on_pauseBtn_clicked() {
    if (game->gameStatus == PLAYING) {
        game->gameStatus = PAUSE;
        gameTimer->stop();
        allBlocksEnable(false);
        allFunBtnEnable(false);
        ui->pauseBtn->setEnabled(true);
        ui->pauseBtn->setText("继续");
    } else {
        game->gameStatus = PLAYING;
        gameTimer->start(kLinkTimerDelay);
        allBlocksEnable(true);
        allFunBtnEnable(true);
        ui->pauseBtn->setText("暂停");
    }
}

void MainGameWindow::on_resetBtn_clicked() {
    //如果有解则扣分
    if (!game->isFrozen()) {
        game->setScore(game->getScore() - 6);
        ui->scoreLab->setText(QString::number(game->getScore()));
    }
    game->reset();
    // 添加button
    init_imageBtn(false);
}

bool MainGameWindow::eventFilter(QObject *watched, QEvent *event) {
    // 重绘时会调用，可以手动调用
    if (event->type() == QEvent::Paint) {
        QPainter painter(ui->centralWidget);
        painter.drawPixmap(rect(), QPixmap(":/res/image/background.jpg"), QRect());
        QPen pen;
        // Random Color
        QColor color(rand() % 256, rand() % 256, rand() % 256);
        pen.setColor(color);
        pen.setWidth(5);
        painter.setPen(pen);
        // 连接各点画线
        for (int i = 0; i < static_cast<int>(game->paintPoints.size()) - 1; i++) {
            PaintPoint p1 = game->paintPoints[i];
            PaintPoint p2 = game->paintPoints[i + 1];
            // 拿到各button的坐标,注意边缘点坐标
            QPoint btn_pos1;
            QPoint btn_pos2;
            // p1
            // 边界情况会导致某点的坐标是-1或者最大值，不可通过数组的线性查找找到对应点，需进行冗余计算
            if (p1.x == -1) {
                btn_pos1 = imageButton[p1.y * MAX_COL + 0]->pos();
                btn_pos1 = QPoint(btn_pos1.x() - kIconSize, btn_pos1.y());
            } else if (p1.x == MAX_COL) {
                btn_pos1 = imageButton[p1.y * MAX_COL + MAX_COL - 1]->pos();
                btn_pos1 = QPoint(btn_pos1.x() + kIconSize, btn_pos1.y());
            } else if (p1.y == -1) {
                btn_pos1 = imageButton[0 + p1.x]->pos();
                btn_pos1 = QPoint(btn_pos1.x(), btn_pos1.y() - kIconSize);
            } else if (p1.y == MAX_ROW) {
                btn_pos1 = imageButton[(MAX_ROW - 1) * MAX_COL + p1.x]->pos();
                btn_pos1 = QPoint(btn_pos1.x(), btn_pos1.y() + kIconSize);
            } else
                btn_pos1 = imageButton[p1.y * MAX_COL + p1.x]->pos();
            // p2
            if (p2.x == -1) {
                btn_pos2 = imageButton[p2.y * MAX_COL + 0]->pos();
                btn_pos2 = QPoint(btn_pos2.x() - kIconSize, btn_pos2.y());
            } else if (p2.x == MAX_COL) {
                btn_pos2 = imageButton[p2.y * MAX_COL + MAX_COL - 1]->pos();
                btn_pos2 = QPoint(btn_pos2.x() + kIconSize, btn_pos2.y());
            } else if (p2.y == -1) {
                btn_pos2 = imageButton[0 + p2.x]->pos();
                btn_pos2 = QPoint(btn_pos2.x(), btn_pos2.y() - kIconSize);
            } else if (p2.y == MAX_ROW) {
                btn_pos2 = imageButton[(MAX_ROW - 1) * MAX_COL + p2.x]->pos();
                btn_pos2 = QPoint(btn_pos2.x(), btn_pos2.y() + kIconSize);
            } else
                btn_pos2 = imageButton[p2.y * MAX_COL + p2.x]->pos();
            // 中心点
            QPoint pos1(btn_pos1.x() + kIconSize / 2, btn_pos1.y() - kIconSize / 2);
            QPoint pos2(btn_pos2.x() + kIconSize / 2, btn_pos2.y() - kIconSize / 2);
            painter.drawLine(pos1, pos2);
        }
        //如果僵局则重绘
        if (game->isFrozen()) {
            game->reset();
            // 添加button
            init_imageBtn(false);
        }
        return true;
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainGameWindow::on_again(GameLevel mode) {
    disconnect(gameTimer, &QTimer::timeout, this, &MainGameWindow::gameTimerEvent);
    delete gameTimer;
    // 先析构之前的
    if (game) {
        delete game;
        for (auto &i: imageButton) {
            delete i;
        }
    }
    // TODO: 停止音乐
    // 重绘
    update();
    initGame(mode);
}

void MainGameWindow::gameOver(bool mode) {
    //停止进度条
    gameTimer->stop();
    // TODO: 记录游戏数据
    QString level;
    switch (curLevel) {
        case BASIC:
            level = "入门";
            break;
        case MEDIUM:
            level = "普通";
            break;
        case HARD:
            level = "困难";
    }
    //创建界面对象
    overDialog dia(mode, game->getScore(), this);
    connect(&dia,SIGNAL(again(GameLevel)), this,SLOT(on_again(GameLevel)));
    connect(&dia,SIGNAL(exit()), this,SLOT(on_exit()));
    dia.exec();
}

void MainGameWindow::on_exit() {
    //TODO: 数据库
    exit(0);
}

void MainGameWindow::informationDisplay() {
    const auto *actionSender = dynamic_cast<QAction *>(sender());
    if (actionSender == ui->actionIntro) {
        QMessageBox::information(this, "简介", "巡旅联觉 - Traveller's Linkage");
    } else if (actionSender == ui->actionGroup) {
        QMessageBox::information(this, "小组成员",
                                 "54shitaimzf      Billybilly233\n"
                                 "lalalangren      Stanley-233(组长)\n"
                                 "(按字典序排序)");
    }
}

void MainGameWindow::createGameWithLevel() {
    disconnect(gameTimer, &QTimer::timeout, this, &MainGameWindow::gameTimerEvent);
    delete gameTimer;
    // 先析构之前的
    if (game) {
        delete game;
        for (const auto &i: imageButton) {
            delete i;
        }
    }
    // 重绘
    update();
    auto *actionSender = dynamic_cast<QAction *>(sender());
    if (actionSender == ui->actionBasic) {
        initGame(BASIC);
    } else if (actionSender == ui->actionMedium) {
        initGame(MEDIUM);
    } else if (actionSender == ui->actionHard) {
        initGame(HARD);
    }
}

void MainGameWindow::on_hintBtn_clicked() {
    if (!game->getHintsLast())
        return;

    // 初始时不能获得提示
    for (int i = 0; i < 4; i++)
        if (game->getHint()[i] == -1)
            return;

    if (!isReallyLinked) {
        game->setHintsLast(game->getHintsLast() - 1);
        isReallyLinked = true;
    }
    ui->hintsLastLab->setText(QString::number(game->getHintsLast()));

    int srcX = game->getHint()[0];
    int srcY = game->getHint()[1];
    int dstX = game->getHint()[2];
    int dstY = game->getHint()[3];

    IconButton *srcIcon = imageButton[srcY * MAX_COL + srcX];
    IconButton *dstIcon = imageButton[dstY * MAX_COL + dstX];
    srcIcon->setStyleSheet(kIconHintStyle);
    dstIcon->setStyleSheet(kIconHintStyle);
}

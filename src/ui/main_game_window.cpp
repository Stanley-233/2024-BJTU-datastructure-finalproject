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

    // BGM
    player.setAudioOutput(&audioOutput);
    player.setSource(QUrl("qrc:/res/sound/bgm.mp3"));
    audioOutput.setVolume(100);
    player.setLoops(-1); // 无限循环
    player.play();
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

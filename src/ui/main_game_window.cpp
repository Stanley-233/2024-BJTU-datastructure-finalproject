#include "main_game_window.h"

#include "login_dialog.h"
#include "ui_main_game_window.h"

MainGameWindow::MainGameWindow(QWidget *parent) :
    QMainWindow(parent)
    , ui(new Ui::MainGameWindow)
    , networkHandler(NetworkHandler(this)) {
    ui->setupUi(this);
    this->setWindowTitle("巡旅联觉 - Traveller's Linkage");

    // 登录逻辑
    LoginDialog lg(this);
    connect(&lg, &LoginDialog::sendingUserMsg, this, &MainGameWindow::onLoginMessage);
    connect(this, &MainGameWindow::login_dialog_message, &lg, &LoginDialog::on_DialogState);
    lg.exec();

    connect(&networkHandler, &NetworkHandler::successfulLogin, this, [this] {
        emit login_dialog_message(true, true, "登录成功！");
    });
    connect(&networkHandler, &NetworkHandler::wrongPassword, this, &MainGameWindow::onWrongPassword);
    connect(&networkHandler, &NetworkHandler::noUser, this, &MainGameWindow::onNoUser);
    connect(&networkHandler, &NetworkHandler::serverError, this, &MainGameWindow::onServerError);
    connect(&networkHandler, &NetworkHandler::successfulRegister, this, &MainGameWindow::onSuccessfulRegister);
    connect(&networkHandler, &NetworkHandler::alreadyRegistered, this, &MainGameWindow::onAlreadyRegistered);
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

void MainGameWindow::onWrongPassword() {
    emit login_dialog_message(false, true, "登陆失败：密码错误！");
}

void MainGameWindow::onNoUser() {
    emit login_dialog_message(false, true, "登陆失败：用户未注册！");
}

void MainGameWindow::onServerError() {
    emit login_dialog_message(false, true, "登陆失败：服务器错误！");
}

void MainGameWindow::onSuccessfulRegister() {
    emit login_dialog_message(true, false, "注册成功！");
}

void MainGameWindow::onAlreadyRegistered() {
    emit login_dialog_message(false, false, "注册失败：已存在用户！");;
}

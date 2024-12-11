//
// Created by stanl on 24-12-11.
//

#include "login_dialog.h"
#include "ui_login_dialog.h"

#include <QPainter>
#include <QMessageBox>

LoginDialog::LoginDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog) {
    ui->setupUi(this);
    setWindowTitle("巡旅联觉：登录");
    tap = new QSoundEffect(this);
    done = new QSoundEffect(this);
    done->setSource(QUrl::fromLocalFile(":/res/sound/success.wav"));
    tap->setSource(QUrl::fromLocalFile(":/res/sound/type.wav"));
    done->setVolume(1.0f); // 最大音量
    tap->setVolume(1.0f);
}

LoginDialog::~LoginDialog() {
    delete ui;
}

void LoginDialog::closeEvent(QCloseEvent *e) {
    exit(0);
}

void LoginDialog::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(":/res/dialog/loginPage.jpg"), QRect());
}


void LoginDialog::on_loginBtn_clicked() {
    QString name = ui->nameEL->text();
    QString password = ui->passwordEL->text();
    if (!name.isEmpty() && !password.isEmpty()) {
        emit sendingUserMsg(1, name, password);
    } else {
        QMessageBox::warning(this, "警告", "请检查输入！");
    }
}


void LoginDialog::on_registerBtn_clicked() {
    QString name = ui->nameEL->text();
    QString password = ui->passwordEL->text();
    if (!name.isEmpty() && !password.isEmpty()) {
        emit sendingUserMsg(0, name, password);
    } else {
        QMessageBox::warning(this, "警告", "请检查输入！");
    }
}

void LoginDialog::on_DialogState(bool isSuccess, bool type, const QString &info) {
    if (type) {
        if (isSuccess) {
            done->play();
            QMessageBox::information(this, "信息", info);
            hide();
        } else {
            QMessageBox::warning(this, "信息", info);
            ui->passwordEL->clear();
        }
    } else {
        if (isSuccess) {
            done->play();
            QMessageBox::information(this, "信息", info);
        } else {
            QMessageBox::warning(this, "警告", info);
            ui->nameEL->clear();
            ui->passwordEL->clear();
        }
    }
}

void LoginDialog::on_nameEL_textEdited(const QString &arg1) const {
    tap->play();
}

void LoginDialog::on_passwordEL_textEdited(const QString &arg1) const {
    tap->play();
}

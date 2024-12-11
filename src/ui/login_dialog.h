//
// Created by stanl on 24-12-11.
//

#ifndef LOGIN_DIALOG_H
#define LOGIN_DIALOG_H

#include <QDialog>
#include <QSoundEffect>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);

    ~LoginDialog() override;

    void closeEvent(QCloseEvent *e) override;

    void paintEvent(QPaintEvent *event) override; //重绘事件

private slots:
    void on_loginBtn_clicked();

    void on_registerBtn_clicked();

    void on_nameEL_textEdited(const QString &arg1) const;

    void on_passwordEL_textEdited(const QString &arg1) const;

public slots:
    void on_DialogState(bool isSuccess, bool type, const QString &info);

signals:
    void sendingUserMsg(bool mode, QString name, QString password);

private:
    Ui::LoginDialog *ui;
    QSoundEffect *tap;
    QSoundEffect *done;
};


#endif //LOGIN_DIALOG_H

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

    QSoundEffect *tap;
    QSoundEffect *done;

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog() override;
    void closeEvent(QCloseEvent *e) override;
    void paintEvent(QPaintEvent *event) override;        //重绘事件

    private slots:
        void on_loginBtn_clicked();

    void on_registerBtn_clicked();
    void on_loginState(bool state);
    void on_registerState(bool state);
    void on_nameEL_textEdited(const QString &arg1);
    void on_passwordEL_textEdited(const QString &arg1);

    signals:
        void sendingUserMsg(bool mode, QString name, QString password);

private:
    Ui::LoginDialog *ui;
};



#endif //LOGIN_DIALOG_H

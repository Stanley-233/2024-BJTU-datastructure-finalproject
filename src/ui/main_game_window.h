#ifndef MAIN_GAME_WINDOW_H
#define MAIN_GAME_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>

#include "../network/network_handler.h"

namespace Ui {
    class MainGameWindow;
}

class MainGameWindow : public QMainWindow {
    Q_OBJECT

public:
    MainGameWindow(QWidget *parent = nullptr);

    ~MainGameWindow() override;

private slots:
    void onLoginMessage(bool mode, QString name, QString password);

signals:
    void login_dialog_message(bool isSuccess, bool mode, QString info);

private:
    Ui::MainGameWindow *ui;
    NetworkHandler networkHandler;
};

#endif // MAIN_GAME_WINDOW_H

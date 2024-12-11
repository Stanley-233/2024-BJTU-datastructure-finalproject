#ifndef MAIN_GAME_WINDOW_H
#define MAIN_GAME_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

#include "../network/network_handler.h"

namespace Ui {
    class MainGameWindow;
}

class MainGameWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainGameWindow(QWidget *parent = nullptr);

    ~MainGameWindow() override;

private slots:
    void onLoginMessage(bool mode, QString name, QString password);

    void on_muteBtn_clicked();

signals:
    void login_dialog_message(bool isSuccess, bool mode, QString info);

private:
    Ui::MainGameWindow *ui;
    NetworkHandler networkHandler;
    QAudioOutput audioOutput;
    QMediaPlayer player;
};

#endif // MAIN_GAME_WINDOW_H

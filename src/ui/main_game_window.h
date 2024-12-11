#ifndef MAIN_GAME_WINDOW_H
#define MAIN_GAME_WINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QMediaPlayer>

namespace Ui {
    class MainGameWindow;
}

class MainGameWindow : public QMainWindow {
    Q_OBJECT

public:
    MainGameWindow(QWidget *parent = nullptr);
    ~MainGameWindow() override;

private:
    Ui::MainGameWindow *ui;
};

#endif // MAIN_GAME_WINDOW_H

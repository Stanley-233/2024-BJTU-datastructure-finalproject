#include "main_game_window.h"
#include "ui_main_game_window.h"

MainGameWindow::MainGameWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainGameWindow)
{
    ui->setupUi(this);
}

MainGameWindow::~MainGameWindow() {
    delete ui;
}

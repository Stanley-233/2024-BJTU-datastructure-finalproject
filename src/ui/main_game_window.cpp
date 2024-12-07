#include "main_game_window.h"
#include "ui_main_game_window.h"

MainGameWindow::MainGameWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainGameWindow) {
    ui->setupUi(this);
    this->setWindowTitle("巡旅联觉 - Traveller's Linkage");
}

MainGameWindow::~MainGameWindow() {
    delete ui;
}

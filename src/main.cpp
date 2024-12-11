#include "ui/main_game_window.h"

#include <QApplication>
#include <QSoundEffect>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    a.setStyleSheet("QWidget { background-color: white; color: black; }");
    MainGameWindow w;
    w.setFixedSize(w.geometry().size());
    w.show();
    return a.exec();
}

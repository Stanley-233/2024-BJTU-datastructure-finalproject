#ifndef OVERDIALOG_H
#define OVERDIALOG_H

#include <QDialog>
#include <QSoundEffect>

#include "../game/game_model.h"

namespace Ui {
    class overDialog;
}

class overDialog : public QDialog
{
    Q_OBJECT

public:
    overDialog(bool mode,int score,QWidget *parent = nullptr);
    ~overDialog() override;

    void closeEvent(QCloseEvent *e) override;

private slots:
    void on_exitBtn_clicked();

    void on_hardBtn_clicked();

    void on_basicBtn_clicked();

    void on_mediumBtn_clicked();

private:
    Ui::overDialog *ui;
    QSoundEffect *win = new QSoundEffect();
    QSoundEffect *lose = new QSoundEffect();

signals:
    void again(GameLevel mode);
    void exit();
};

#endif // OVERDIALOG_H

//
// Created by stanl on 24-12-22.
//

#ifndef RANKING_DIALOG_H
#define RANKING_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE

namespace Ui {
    class RankDialog;
}

QT_END_NAMESPACE

class RankDialog : public QDialog {
    Q_OBJECT

public:
    explicit RankDialog(QWidget *parent, const QString& message);
    ~RankDialog() override;

private slots:
    void on_returnButton_clicked();

private:
    Ui::RankDialog *ui;
    void paintEvent(QPaintEvent *event) override;
};


#endif //RANKING_DIALOG_H

//
// Created by stanl on 24-12-22.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ranking_dialog.h" resolved

#include "ranking_dialog.h"

#include <QPainter>

#include "ui_ranking_dialog.h"


RankDialog::RankDialog(QWidget *parent, const QString& message) :
    QDialog(parent), ui(new Ui::RankDialog) {
    ui->setupUi(this);
    ui->textEdit->setPlainText(message);
    ui->textEdit->setAlignment(Qt::AlignCenter);
    ui->textEdit->setTextColor(Qt::white);
    setWindowIcon(QIcon(":/res/image/icon.ico"));
}

RankDialog::~RankDialog() {
    delete ui;
}

void RankDialog::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(rect(), QPixmap(":/res/dialog/loginPage.jpg"), QRect());
}

void RankDialog::on_returnButton_clicked() {
    this->close();
}

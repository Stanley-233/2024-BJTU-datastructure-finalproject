#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "ranking_record.h"

class NetworkHandler final : public QObject {
    Q_OBJECT

public:
    explicit NetworkHandler(QObject *parent = nullptr);
    void login(const QString &username, const QString &password);
    void registerUser(const QString &username, const QString &password) const;
    void getSeed();
    void putRank(int score, int time);
    void getRank();
    const QString & getUserName() const;

signals:
    void successfulLogin();
    void wrongPassword();
    void noUser();
    void serverError();
    void successfulRegister();
    void alreadyRegistered();
    void seed(qint64);
    void rankUpdated();
    void rankUnchanged();
    void rank(RankingRecord*);
    void noRank();

private slots:
    void onNetworkReplay(QNetworkReply *reply);

private:
    QNetworkAccessManager *networkManager;
    QString username = QString("");
};

#endif // NETWORKHANDLER_H

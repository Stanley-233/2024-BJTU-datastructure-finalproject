#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QString>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class NetworkHandler : public QObject {
    Q_OBJECT

public:
    explicit NetworkHandler(QObject *parent = nullptr);

    void login(const QString &username, const QString &password) const;

    void registerUser(const QString &username, const QString &password) const;

signals:
    void successfulLogin();

    void wrongPassword();

    void noUser();

    void serverError();

    void successfulRegister();

    void alreadyRegistered();

private slots:
    void onNetworkReplay(QNetworkReply *reply);

private:
    QString apiUrl = "http://127.0.0.1:5000/";
    QNetworkAccessManager *networkManager;
};

#endif // NETWORKHANDLER_H

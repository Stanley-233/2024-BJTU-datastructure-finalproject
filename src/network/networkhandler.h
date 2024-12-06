#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>

class NetworkHandler : public QObject {
    Q_OBJECT

public:
    explicit NetworkHandler(QObject *parent = nullptr);
    void login(const QString& username, const QString& password) const;
    void registerUser(const QString& username, const QString& password) const;

    signals:
    void successfulLogin();
    void wrongPassword();
    void noUser();
    void serverError();
    void successfulRegister();
    void alreadyRegistered();

    private slots:
    void onLoginReply(QNetworkReply* reply);
    void onRegisterReply(QNetworkReply* reply);

private:
    const QUrl apiUrl = QString("http://127.0.0.1:5000/login");
    QNetworkAccessManager* networkManager;
};

#endif // NETWORKHANDLER_H
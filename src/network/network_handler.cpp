#include "network_handler.h"

// TODO: Test login and register

NetworkHandler::NetworkHandler(QObject *parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)) {
    connect(networkManager, &QNetworkAccessManager::finished, this, &NetworkHandler::onNetworkReplay);
}

void NetworkHandler::login(const QString &username, const QString &password) const {
    // TODO: Change it to deployment
    QNetworkRequest request(QUrl("http://127.0.0.1:5000/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, data);
    reply->setProperty("requestType", "login");
}

void NetworkHandler::registerUser(const QString &username, const QString &password) const {
    // TODO: Change it to deployment
    QNetworkRequest request(QUrl("http://127.0.0.1:5000/register"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, data);
    reply->setProperty("requestType", "register");
}

void NetworkHandler::onNetworkReplay(QNetworkReply *reply) {
    QString requestType = reply->property("requestType").toString();
    if (requestType == "login") {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit successfulLogin();
        } else {
            const int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            switch (statusCode) {
                case 404:
                    emit noUser();
                    break;
                case 403:
                    emit wrongPassword();
                    break;
                default:
                    emit serverError();
                    break;
            }
        }
    } else if (requestType == "register") {
        if (reply->error() == QNetworkReply::NoError) {
            emit successfulRegister();
        } else {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            switch (statusCode) {
                case 409:
                    emit alreadyRegistered();
                    break;
                default:
                    emit serverError();
                    break;
            }
        }
    }
    reply->deleteLater();
}

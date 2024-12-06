#include "NetworkHandler.h"

NetworkHandler::NetworkHandler(QObject *parent) 
    : QObject(parent), networkManager(new QNetworkAccessManager(this)) {
    connect(networkManager, &QNetworkAccessManager::finished, this, &NetworkHandler::onLoginReply);
}

void NetworkHandler::login(const QString& username, const QString& password) const {
    QNetworkRequest request(QUrl("http://127.0.0.1:5000/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["name"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    networkManager->post(request, data);
}

void NetworkHandler::onLoginReply(QNetworkReply* reply) {
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
    reply->deleteLater();
}

void NetworkHandler::registerUser(const QString& username, const QString& password) const {
    QNetworkRequest request(QUrl("http://127.0.0.1:5000/register"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    networkManager->post(request, data);
}

void NetworkHandler::onRegisterReply(QNetworkReply *reply) {
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
    reply->deleteLater();
}

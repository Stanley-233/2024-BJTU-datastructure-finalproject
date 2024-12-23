#include "network_handler.h"

// TODO: Test login and register

NetworkHandler::NetworkHandler(QObject *parent) :
    QObject(parent), networkManager(new QNetworkAccessManager(this)) {
    connect(networkManager, &QNetworkAccessManager::finished, this,
            &NetworkHandler::onNetworkReplay);
}

void NetworkHandler::login(const QString &username,
                           const QString &password) {
    // Changed it to deployment
    QNetworkRequest request(QUrl("http://llk.bearingwall.top/login"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    this->username = username;
    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, data);
    reply->setProperty("requestType", "login");
}

void NetworkHandler::registerUser(const QString &username,
                                  const QString &password) const {
    // Changed it to deployment
    QNetworkRequest request(QUrl("http://llk.bearingwall.top/register"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json;
    json["username"] = username;
    json["password"] = password;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, data);
    reply->setProperty("requestType", "register");
}

void NetworkHandler::getSeed() {
    // Changed it to deployment
    QNetworkRequest request(QUrl("http://llk.bearingwall.top/seed"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager->get(request);
    reply->setProperty("requestType", "seed");
}

void NetworkHandler::putRank(int score, int time) {
    QNetworkRequest request(QUrl("http://llk.bearingwall.top/put_rank"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;
    json["username"] = username;
    json["score"] = score;
    json["time"] = time;

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, data);
    reply->setProperty("requestType", "put_rank");
}

void NetworkHandler::getRank() {
    QNetworkRequest request(QUrl("http://llk.bearingwall.top/get_rank"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QJsonObject json;
    json["username"] = username;
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();
    QNetworkReply *reply = networkManager->post(request, data);
    reply->setProperty("requestType", "get_rank");
}

void NetworkHandler::onNetworkReplay(QNetworkReply *reply) {
    QString requestType = reply->property("requestType").toString();
    if (requestType == "login") {
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit successfulLogin();
        } else {
            const int statusCode = reply->attribute(
                    QNetworkRequest::HttpStatusCodeAttribute).toInt();
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
    }
    else if (requestType == "register") {
        if (reply->error() == QNetworkReply::NoError) {
            emit successfulRegister();
        } else {
            int statusCode = reply->attribute(
                    QNetworkRequest::HttpStatusCodeAttribute).toInt();
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
    else if (requestType == "seed") {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.isObject()) {
                QJsonObject jsonObj = jsonDoc.object();
                const qint64 netSeed = jsonObj["seed"].toVariant().toLongLong();
                emit seed(netSeed);
            }
        } else {
            // 处理错误
            emit serverError();
        }
    }
    else if (requestType == "put_rank") {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 200) {
            emit rankUnchanged();
        } else if (statusCode == 201) {
            emit rankUpdated();
        } else {
            emit serverError();
        }
    }
    else if (requestType == "get_rank") {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 404) {
            emit noRank();
        } else if (statusCode == 200) {
            const auto record = new RankingRecord();
            const QByteArray responseData = reply->readAll();
            const QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            QJsonObject jsonObj = jsonDoc.object();
            // 获取当前用户的排名
            record->rank = jsonObj["rank"].toVariant().toInt();
            // 处理 top_players
            QJsonArray topUsersArray = jsonObj["top_users"].toArray();
            for (const auto &value : topUsersArray) {
                QJsonObject playerObj = value.toObject();
                auto player = new RankingRecord::Player();
                player->name = playerObj["username"].toString();
                player->score = playerObj["score"].toVariant().toInt();
                player->time = playerObj["time"].toVariant().toInt();
                record->top_players.push_back(player);
            }
            emit rank(record);
        } else {
            emit serverError();
        }
    }
    reply->deleteLater();
}

const QString & NetworkHandler::getUserName() const{
    return username;
}

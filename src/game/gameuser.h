//
// Created by stanl on 24-12-6.
//

#ifndef GAMEUSER_H
#define GAMEUSER_H

#include <QString>
#include <utility>

struct GameUser {
    QString name;
    QString password;

    GameUser(QString name, QString password)
        : name(std::move(name)),
          password(std::move(password)) {
    }
};



#endif //GAMEUSER_H

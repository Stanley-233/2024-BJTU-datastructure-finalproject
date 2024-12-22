//
// Created by stanl on 24-12-22.
//

#ifndef RANKING_RECORD_H
#define RANKING_RECORD_H
#include <QString>


struct RankingRecord {
    int rank;
    struct Player {
        QString name;
        int score;
        int time;
    };
    std::vector<Player*> top_players;
};

#endif //RANKING_RECORD_H

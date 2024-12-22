#ifndef USER_DATABASE_H
#define USER_DATABASE_H


#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

typedef struct
{
    QString account;
    QString level;
    int score;
}record;

class User_database
{
public:
    User_database();

    // 打开数据库
    bool openDb(void);
    // 创建数据表
    void createTable(QString account);
    // 判断数据表是否存在
    bool isTableExist(QString account);
    // 查询全部数
    QSqlQuery queryTable(QString account);
    // 插入数据
    void singleInsertData(record &singleData); // 插入单条数据

    // 关闭数据库
    void closeDb(void);

private:
    QSqlDatabase database;// 用于建立和数据库的连接
};



#endif // USER_DATABASE_H

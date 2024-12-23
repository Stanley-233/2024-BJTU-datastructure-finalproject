#include "user_database.h"

// 构造函数中初始化数据库对象，并建立数据库
User_database::User_database() {
    if (QSqlDatabase::contains("qt_sql_default_connection")) {
        database = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        // 建立和SQlite数据库的连接
        database = QSqlDatabase::addDatabase("QSQLITE");
        // 设置数据库文件的名字
        database.setDatabaseName("game_project.db");
    }
}

// 打开数据库
bool User_database::openDb() {
    if (!database.open()) {
        qDebug() << "Error: Failed to connect database." << database.lastError();
    }
    return true;
}

// 创建数据表
void User_database::createTable(QString account) {
    if (isTableExist(account))
        return;
    else {
        // 用于执行sql语句的对象
        QSqlQuery sqlQuery;
        // 构建创建数据库的sql语句字符串
        QString createSql = QString("CREATE TABLE " + account + " (\
                              number INT PRIMARY KEY NOT NULL,\
                              level TEXT NOT NULL,\
                              score INT NOT NULL)");
        sqlQuery.prepare(createSql);
        // 执行sql语句
        if (!sqlQuery.exec()) {
            qDebug() << "Error: Fail to create table. " << sqlQuery.lastError();
        }

    }
}

// 判断数据库中某个数据表是否存在
bool User_database::isTableExist(QString account) {
    QSqlDatabase database = QSqlDatabase::database();
    if (database.tables().contains(account)) {
        return true;
    }
    return false;
}

// 查询全部数据
QSqlQuery User_database::queryTable(QString account) {
    QSqlQuery sqlQuery;
    sqlQuery.exec("SELECT * FROM " + account);
    if (!sqlQuery.exec()) {
        qDebug() << "Error: Fail to query table. " << sqlQuery.lastError();
    } else {

        return sqlQuery;
    }
}

// 插入单条数据
void User_database::singleInsertData(record &singledb) {
    QSqlQuery sqlQuery;
    sqlQuery.prepare("INSERT INTO " + singledb.account + " VALUES(:number,:level,:score)");
    //统计数据数量
    QSqlQuery temp = queryTable(singledb.account);
    int sum = 0;
    while (temp.next())
        sum++;
    sqlQuery.bindValue(":number", sum + 1);
    sqlQuery.bindValue(":level", singledb.level);
    sqlQuery.bindValue(":score", singledb.score);
    if (!sqlQuery.exec()) {
        qDebug() << "Error: Fail to insert data. " << sqlQuery.lastError();
    }

}

void User_database::closeDb(void) {
    database.close();
}

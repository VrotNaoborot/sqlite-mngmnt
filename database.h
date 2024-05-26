#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include <string>

class Database {
public:
    Database();
    QSqlDatabase connect_to_db(const std::string &dbName);
    bool createTestDatabase(const std::string &dbName);
};

#endif // DATABASE_H

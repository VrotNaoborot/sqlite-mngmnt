#ifndef DATABASE_H
#define DATABASE_H

#include <QMainWindow>

class Database
{
public:
    Database();
    bool createDatabase(const std::string &db_name);
    QSqlDatabase connect_to_db(const std::string &db_name);
};

#endif // DATABASE_H

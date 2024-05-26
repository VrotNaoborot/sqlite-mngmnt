#include <iostream>
#include <string>
#include <QtSql>
#include "database.h"

Database::Database() {
    // Пустая реализация конструктора
}


// Подключается или создает бд и подключается к бд, если он не найдено
QSqlDatabase Database::connect_to_db(const std::string &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(dbName));

    if (!db.open()) {
        std::cerr << "Ошибка при подключении к базе данных: " << db.lastError().text().toStdString() << std::endl;
        return QSqlDatabase();
    }

    return db;
}

bool Database::createTestDatabase(const std::string &dbName) {
    QSqlDatabase db = connect_to_db(dbName);

    QString createTableQuery = "CREATE TABLE IF NOT EXISTS test ("
                               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "Name VARCHAR(255)"
                               ");";
    QSqlQuery createTableSqlQuery(db);
    if (!createTableSqlQuery.exec(createTableQuery)) {
        std::cerr << "Ошибка при создании таблицы test: " << createTableSqlQuery.lastError().text().toStdString() << std::endl;
        db.close();
        return false;
    }

    QString insertDataQuery = "INSERT INTO test (Name) VALUES ('TestName1'), ('TestName2'), ('TestName3');";
    QSqlQuery insertDataSqlQuery(db);
    if (!insertDataSqlQuery.exec(insertDataQuery)) {
        std::cerr << "Ошибка при вставке тестовых данных: " << insertDataSqlQuery.lastError().text().toStdString() << std::endl;
        db.close();
        return false;
    }

    db.close();

    std::cout << "Тестовая бд 'test' с таблицей 'test' создана" << std::endl;
    return true;
}

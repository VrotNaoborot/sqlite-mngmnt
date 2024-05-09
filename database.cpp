#include <iostream>
#include <string>
#include <QtSql>
#include <QCoreApplication>
// #include <windows.h>
#include <fstream>

#include "database.h"
#include "fileworker.h"


Database::Database() {
    // Пустая реализация пустого конструктора
}

QSqlDatabase Database::connect_to_db(const std::string &dbName) {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QString::fromStdString(dbName));

    // Попытка подключения к базе данных
    if (!db.open()) {
        std::cerr << "Ошибка при подключении к базе данных: " << db.lastError().text().toStdString() << std::endl;
        return QSqlDatabase(); // Возвращаем пустой объект QSqlDatabase
    }

    std::cout << "Подключение к базе данных успешно: " << dbName << std::endl;
    return db;
}

bool Database::createDatabase(const std::string &dbName) {
    Database db_obj;
    // получение объекта базы данных
    QSqlDatabase db = db_obj.connect_to_db(dbName);

    QString createTableQuery = "CREATE TABLE IF NOT EXISTS TestTable ("
                               "ID INTEGER PRIMARY KEY AUTOINCREMENT,"
                               "Name VARCHAR(255)"
                               ");";
    QSqlQuery createTableSqlQuery(db);
    if (!createTableSqlQuery.exec(createTableQuery)) {
        std::cerr << "Ошибка при создании таблицы TestTable: " << createTableSqlQuery.lastError().text().toStdString() << std::endl;
        db.close();
        return false;
    }

    // Вставляем тестовые данные
    QString insertDataQuery = "INSERT INTO TestTable (Name) VALUES ('TestName1'), ('TestName2'), ('TestName3');";
    QSqlQuery insertDataSqlQuery(db);
    if (!insertDataSqlQuery.exec(insertDataQuery)) {
        std::cerr << "Ошибка при вставке тестовых данных: " << insertDataSqlQuery.lastError().text().toStdString() << std::endl;
        db.close();
        return false;
    }

    // Закрытие базы данных
    db.close();

    std::cout << "База данных успешно создана: " << dbName << std::endl;
    return true;
}





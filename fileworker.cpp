#include "fileworker.h"
#include <iostream>
#include <string>
#include <QtSql>
#include <QCoreApplication>
// #include <windows.h>
#include <QFile>
#include <QString>



using namespace std;

#include "fileworker.h"

FileWorker::FileWorker() {
    // Пустая реализация пустого конструктора
}


bool FileWorker::fileExists(const QString &fileName) {
    QFile file(fileName);
    return file.exists();
}

std::string FileWorker::get_path() {
    // ф-ция для получения названия бд
    string pth;
    while (true) {
        std::cout << "Введите название файла (с .db)" << std::endl;
        std::getline(std::cin, pth);
        if (!fileExists(QString::fromStdString(pth))) {
            std::cerr << "Файл базы данных с названием '" << pth << "' не существует" << std::endl;
        }
        else{
            return pth;
        }
    }
}

QString FileWorker::generate_insert(const QString &tableName, const QStringList &columns, const QStringList &values) {
    QString query = "INSERT INTO " + tableName + " (";

    // Формируем список столбцов
    query += columns.join(", ") + ") VALUES (";

    // Формируем список значений
    for (int i = 0; i < values.size(); ++i) {
        if (i > 0) {
            query += ", ";
        }

        // Если значение - строка, добавляем кавычки
        if (values[i].startsWith('\"') && values[i].endsWith('\"')) {
            query += values[i];
        } else {
            query += "'" + values[i] + "'";
        }
    }

    query += ");";
    return query;
}

QStringList FileWorker::convert_to_list(const QString &line) {
    QStringList fields;
    QString field;
    bool insideQuotes = false;

    for (const QChar &ch : line) {
        if (ch == ';' && !insideQuotes) {
            fields.append(field.trimmed());
            field.clear();
        } else {
            field.append(ch);
            if (ch == '"')
                insideQuotes = !insideQuotes;
        }
    }

    fields.append(field.trimmed()); // добавляем последнее поле
    return fields;
}

void FileWorker::parse_csv(const QString &filePath, const QString &tableName, QSqlDatabase &database) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Не удалось открыть файл: " << file.errorString().toStdString() << std::endl;
        file.close();
        return;
    }

    QTextStream in(&file);
    QString headerLine = in.readLine();
    QStringList columns = convert_to_list(headerLine);

    // создаем таблицу, если она не существует
    QString createTableQuery = "CREATE TABLE IF NOT EXISTS " + tableName + " (";
    for (int i = 0; i < columns.size(); ++i) {
        createTableQuery += columns[i];

        // Проверяем, если столбец - ID
        if (columns[i] == "ID") {
            createTableQuery += " INTEGER PRIMARY KEY";
        } else {
            createTableQuery += " TEXT";
        }

        // Если это последний столбец, не добавляем запятую
        if (i < columns.size() - 1) {
            createTableQuery += ", ";
        }
    }
    createTableQuery += ");";
    std::cout << "Запрос на создание таблицы: " << createTableQuery.toStdString() << std::endl;


    QSqlQuery createTableSqlQuery(database);
    if (!createTableSqlQuery.exec(createTableQuery)) {
        std::cerr << "Ошибка при создании таблицы" << createTableSqlQuery.lastError().text().toStdString() << std::endl;
        return;
    }

    while (!in.atEnd()) {
        QString dataLine = in.readLine();
        QStringList values = convert_to_list(dataLine);

        QString query = generate_insert(tableName, columns, values);
        std::cout << query.toStdString() << std::endl;

        QSqlQuery sqlQuery(database);
        if (!sqlQuery.exec(query)) {
            std::cerr << "Ошибка при выполнении запроса:" << sqlQuery.lastError().text().toStdString() << std::endl;
            file.close();
            return;
        }
    }

    file.close();
};



#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDebug>
#include <iostream>
#include <QtSql>


using namespace std;

QString generate_insert(const QString &tableName, const QStringList &columns, const QStringList &values)
{
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


QStringList convert_to_list(const QString &line)
{
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

void parse_csv(const QString &filePath, const QString &tableName, QSqlDatabase &database)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Failed to open file:" << file.errorString().toStdString() << std::endl;
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
}

// int main()
// {
//     string filePath; // Укажите путь к вашему CSV файлу
//     std::cout << "Путь до .csv :" << std::endl;
//     std::getline(std::cin, filePath);
//     QString qPath = QString::fromStdString(filePath);

//     QString tableName = "my_table"; // Имя вашей таблицы

//     parse_csv(qPath, tableName);

//     return 0;
// }

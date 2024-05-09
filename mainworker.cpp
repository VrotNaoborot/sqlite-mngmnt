#include <iostream>
#include <string>
#include <QtSql>
#include <QCoreApplication>
// #include <windows.h>
#include <fstream>

#include "database.h"
#include "fileworker.h"

using namespace std;

#include "mainworker.h"

MainWorker::MainWorker() {

}

void MainWorker::execute_command(QSqlDatabase db) {
    std::string command;

    while (true) {
        std::cout << "Введите команду (или 'exit' для завершения): ";
        std::getline(std::cin, command);

        // Находим первое слово в команде
        std::string::size_type spacePos = command.find(' ');
        std::string firstWord = command.substr(0, spacePos);
        // Преобразуем первое слово в верхний регистр
        std::transform(firstWord.begin(), firstWord.end(), firstWord.begin(), ::toupper);

        // Создаем новую команду, где только первое слово в верхнем регистре
        std::string correctedCommand = firstWord + command.substr(spacePos);

        if (correctedCommand == "EXIT") {
            std::cout << "Завершение программы." << std::endl;
            db.close();
            break;
        }

        if (correctedCommand.find("WRITE") == 0) {
            // Получаем путь к файлу из команды
            size_t spaceIndex = correctedCommand.find(" ");
            if (spaceIndex != std::string::npos && spaceIndex + 1 < correctedCommand.size()) {
                std::string filePath = correctedCommand.substr(spaceIndex + 1);

                // Парсим CSV файл и добавляем данные в базу данных
                QString qFilePath = QString::fromStdString(filePath);

                string table;
                std::cout << "Введите название для таблицы: " << std::endl;
                std::getline(std::cin, table);
                QString qTable = QString::fromStdString(table);

                FileWorker fileWorker;
                fileWorker.parse_csv(qFilePath, qTable, db);
            } else {
                std::cerr << "Некорректный формат команды write. Пример: write filename.csv" << std::endl;
            }
        } else if (correctedCommand == "EXIT") {
            std::cout << "Завершение программы." << std::endl;
        } else {
            // Выполняем SQL-запрос
            QSqlQuery query(db);
            if (!query.exec(QString::fromStdString(correctedCommand))) {
                std::cerr << "Ошибка выполнения запроса: " << query.lastError().text().toStdString() << std::endl;
            } else {
                std::cout << "Запрос выполнен успешно." << std::endl;

                // Если запрос начинается с SELECT, выводим результат
                if (correctedCommand.find("SELECT") == 0) {
                    // Выводим названия столбцов
                    QSqlRecord record = query.record();
                    for (int i = 0; i < record.count(); ++i) {
                        std::cout << record.fieldName(i).toStdString() << "\t";
                    }
                    std::cout << std::endl;

                    // Выводим значения столбцов
                    while (query.next()) {
                        for (int i = 0; i < record.count(); ++i) {
                            std::cout << query.value(i).toString().toStdString() << "\t";
                        }
                        std::cout << std::endl;
                    }
                }
            }
        }
    }
}

void MainWorker::start() {
    string csv_name;
    string table_name;
    string db_name;

    // Создание объекта для работы с базой данных SQLite
    Database db_obj;

    // Создание объекта для работы с файлами
    FileWorker fileworker_obj;

    // Создание базы данных
    db_obj.createDatabase("test.db");

    // Получение пути к базе данных
    db_name = fileworker_obj.get_path();

    // Подключение к базе данных
    QSqlDatabase db = db_obj.connect_to_db(db_name);

    // Проверка успешности подключения
    if (!db.isValid()) {
        std::cout << "Перезапустите приложение" << std::endl;
        db.close();
        return;
    }

    MainWorker mainWorker;
    mainWorker.execute_command(db);

}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    MainWorker mainWorker;
    mainWorker.start();

    return app.exec();
}



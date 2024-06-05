#include <iostream>
#include <string>
#include <QtSql>
#include <QCoreApplication>
#include <algorithm>
#include <sstream>

#include "database.h"
#include "fileworker.h"
#include "mainworker.h"

#include <windows.h>
#include <io.h>
#include <fcntl.h>

MainWorker::MainWorker() {}

void MainWorker::execute_command(QSqlDatabase db) {
    std::string command;

    while (true) {
        std::cout << "Введите команду (или 'exit' для завершения): ";
        std::getline(std::cin, command);

        // Используем stringstream для разбиения строки на отдельные слова
        std::istringstream iss(command);
        std::string firstWord;
        iss >> firstWord;

        // Преобразуем первое слово в верхний регистр
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);

        if (command == "EXIT") {
            std::cout << "Завершение программы." << std::endl;
            db.close();
            break;
        }

        if (command.find("WRITE") == 0) {
            // Получаем путь к файлу из команды
            size_t spaceIndex = command.find(" ");
            if (spaceIndex != std::string::npos && spaceIndex + 1 < command.size()) {
                std::string filePath = command.substr(spaceIndex + 1);

                // Парсим CSV файл и добавляем данные в базу данных
                QString qFilePath = QString::fromStdString(filePath);

                std::string table;
                std::cout << "Введите название для таблицы: " << std::endl;
                std::getline(std::cin, table);
                QString qTable = QString::fromStdString(table);

                FileWorker fileWorker;
                fileWorker.parse_csv(qFilePath, qTable, db);
            }

        } else if (command == "EXIT") {
            std::cout << "Завершение программы." << std::endl;
        } else {
            // Выполняем SQL-запрос
            QSqlQuery query(db);
            if (!query.exec(QString::fromStdString(command))) {
                std::cerr << "Ошибка выполнения запроса: " << query.lastError().text().toStdString() << std::endl;
            } else {
                std::cout << "Запрос выполнен успешно." << std::endl;

                // Если запрос начинается с SELECT, выводим результат
                if (command.find("SELECT") == 0) {
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


int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    // Устанавливаем кодировку UTF-8 для консоли
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    std::string db_name;

    // Создание объекта для работы с базой данных SQLite
    Database db_obj;

    // Создание базы данных
    db_obj.createTestDatabase("test.db");

    // Создание объекта для работы с файлами
    FileWorker fileworker_obj;

    // Получение пути к базе данных
    db_name = fileworker_obj.get_path();

    // Проверка успешности подключения
    QSqlDatabase db = db_obj.connect_to_db(db_name);
    if (!db.isValid()) {
        std::cout << "Перезапустите приложение" << std::endl;
        db.close();
    }

    MainWorker mainWorker;
    mainWorker.execute_command(db);

    return app.exec();
}

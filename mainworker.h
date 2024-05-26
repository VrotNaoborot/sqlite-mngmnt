#ifndef MAINWORKER_H
#define MAINWORKER_H

#include <QtSql>

class MainWorker {
public:
    MainWorker();
    void execute_command(QSqlDatabase db);
    void start();
};

#endif // MAINWORKER_H

#ifndef MAINWORKER_H
#define MAINWORKER_H

class MainWorker
{
public:
    MainWorker();
    void start();
    void execute_command(QSqlDatabase db);
};

#endif // MAINWORKER_H

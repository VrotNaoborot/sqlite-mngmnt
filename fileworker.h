#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <QtSql>
#include <QString>
#include <QStringList>

class FileWorker {
public:
    FileWorker();
    bool fileExists(const QString &fileName);
    std::string get_path();
    QString generate_insert(const QString &tableName, const QStringList &columns, const QStringList &values);
    QStringList convert_to_list(const QString &line);
    void parse_csv(const QString &filePath, const QString &tableName, QSqlDatabase &database);
};

#endif // FILEWORKER_H

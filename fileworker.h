#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <Qstring>
#include <QtSql>

using namespace std;

class FileWorker {
public:
    FileWorker();
    bool file_exists(const std::string& filename);
    std::string get_path();
    QString generate_insert(const QString &tableName, const QStringList &columns, const QStringList &values);
    QStringList convert_to_list(const QString &line);
    void parse_csv(const QString &filePath, const QString &tableName, QSqlDatabase &database);
};

#endif // FILEWORKER_H

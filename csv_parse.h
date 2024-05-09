#ifndef CSV_PARSE_H
#define CSV_PARSE_H

void parse_csv(const QString &filePath, const QString &tableName, QSqlDatabase &database);
QStringList convert_to_list(const QString &line);
QString generate_insert(const QString &tableName, const QStringList &columns, const QStringList &values);


#endif // CSV_PARSE_H

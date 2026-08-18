#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QString>

class Database
{
public:
    Database();

    // открываем БД
    bool open();
    // закрываем БД
    void close();
    // проверяем состояние БД
    bool isOpen() const;
    // получаем содеинение с БД
    QSqlDatabase database() const;

    // получаем путь БД для внешнего использования
    QString databasePath() const;

private:
    // Создаем БД
    QSqlDatabase db;
    // получаем путь к БД
    QString getDatabasePath() const;
};





#endif //DATABASE_H
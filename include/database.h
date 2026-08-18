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
    // получаем соединение с БД
    QSqlDatabase getDatabase() const;

    // получаем путь БД для внешнего использования
    QString databasePath() const;
    // метод начала транзакции
    bool transaction();
    // метод коммита изменений
    bool commit();
    // метод отката изменений
    bool rollback();

    // метод получения ошибки
    QString lastError() const;


private:
    // Создаем БД
    QSqlDatabase db;
    // получаем путь к БД
    QString getDatabasePath() const;
};





#endif //DATABASE_H
#include "database.h"

#include <QDir>
#include <QFileInfo>
#include <QSqlError>
#include <QDebug>


// конструктор класса
Database::Database()
{
    // указываем тип БД
    db = QSqlDatabase::addDatabase("QSQLITE");
}

// метод открытия БД
bool Database::open()
{
    // получаем путь к БД
    const QString path = getDatabasePath();
    // выводим в консоль полученный путь
    qDebug() << "Path of the DB: " << path;
    // если путя не существует
    if (!QFileInfo::exists(path))
    {
        // указываем сообщение и прерываем метод
        qDebug() << "Database file not found:" << path;
        return false;
    }
    // открываем БД по полученному пути
    db.setDatabaseName(path);
    // если БД не открылась
    if (!db.open())
    {
        // выводим сообщение в консоль и указываем саму ошибку, прерывая метод
        qDebug() << "Error opening database:";
        qDebug() << db.lastError().text();

        return false;
    }
    // выводим успешное открытие БД
    qDebug() << "The database was opened successfully";

    return true;
}

// метод закрытия БД
void Database::close()
{
    // проверяем, что соединение имеется, и закрываем
    if (db.isOpen())
    {
        db.close();
    }
}

// метод проверки состояния БД
bool Database::isOpen() const
{
    return db.isOpen();
}

// метод получения БД
QSqlDatabase Database::getDatabase() const
{
    return db;
}

// метод получения пути к БД
QString Database::databasePath() const
{
    return db.databaseName();
}

// метод начала транзакции
bool Database::transaction()
{
    return db.transaction();
}

// метод коммита изменений
bool Database::commit()
{
    return db.commit();
}

// метод отката изменений
bool Database::rollback()
{
    return db.rollback();
}

// метод получения последней ошибки
QString Database::lastError() const
{
    return db.lastError().text();
}

// приватный метод получения пути к БД
QString Database::getDatabasePath() const
{
    return QDir(QStringLiteral(PROJECT_ROOT)).filePath("db/equipment_db.db");
}




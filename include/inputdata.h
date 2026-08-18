#pragma once


#ifndef INPUTDATA_H
#define INPUTDATA_H

#include <QDialog>
#include <QString>
// для выпадающего списка
#include <QComboBox>
// для sql запросов
#include <QSqlQuery>
// для окон с ошибками и информацией
#include <QMessageBox>
// для вывода ошибок
#include <QSqlError>
// подключаем класс БД
#include "database.h"
#include <QList>

namespace Ui {
class InputData;
}

class InputData : public QDialog
{
    Q_OBJECT

public:
    explicit InputData(QWidget *parent = nullptr);
    ~InputData();

    // методы геттер для получения текста с полей
    QString getName() const;

    // метод получения индекса предприятия
    int getMaintanceId() const;

    // метод получения уровней напряжения
    QList<int> getVoltage() const;

private slots:
    void on_btnClose_clicked();

    void on_btnSave_clicked();

private:
    Ui::InputData *ui;

    void loadMaintanceName();



};

#endif // INPUTDATA_H

#pragma once

#ifndef INPUTCONNECTION_H
#define INPUTCONNECTION_H

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


QT_BEGIN_NAMESPACE

namespace Ui
{
    class InputConnection;
}

QT_END_NAMESPACE

class InputConnection : public QDialog
{
    Q_OBJECT

public:
    explicit InputConnection(
        int inSubstationId,
        Database *inDb,
        QWidget *parent = nullptr);

    ~InputConnection() override;


private:
    Ui::InputConnection *ui;

    Database *db;
    int substationId;

    // метод загрузки типов присоединения
    void loadConnectionTypes() const;
    // метод загрузки уровней напряжения
    void loadVoltageLevel() const;

private slots:
    // слот нажатия кнопки отмены
    void on_btnClose_clicked();
    // слот нажатия кнопки сохранения
    void on_btnSave_clicked();

};


#endif //INPUTCONNECTION_H

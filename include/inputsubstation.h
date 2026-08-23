#pragma once

#ifndef INPUTSUBSTATION_H
#define NPUTSUBSTATION_H

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
    class InputSubstation;
}

QT_END_NAMESPACE

class InputSubstation : public QDialog
{
    Q_OBJECT

public:
    explicit InputSubstation(QWidget *parent = nullptr);

    ~InputSubstation() override;

    // методы геттер для получения текста с полей
    QString getName() const;

    // метод получения индекса предприятия
    int getEnterpriseId() const;

    // метод получения уровней напряжения
    QList<int> getVoltage() const;

private slots:
    // слот нажатия кнопки отмены
    void on_btnClose_clicked();
    // слот нажатия кнопки сохранить
    void on_btnSave_clicked();


private:
    Ui::InputSubstation *ui;
    // метод загрузки названий предприятий
    void loadEnterpriseName();
};


#endif //NPUTSUBSTATION_H

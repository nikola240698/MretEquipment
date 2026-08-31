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
#include <QTimer>


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

private slots:
    // слот при выборе типа присоединения для отображения возможных напряжений
    void on_typeBox_currentIndexChanged(int index);
    // слот нажатия кнопки отмены
    void on_btnClose_clicked();
    // слот нажатия кнопки сохранения
    void on_btnSave_clicked();

private:
    Ui::InputConnection *ui;

    Database *db;
    int substationId;

    // динамическое создание checkBox для выбора уровней напряжения
    QList<QCheckBox *> voltageCheckBoxes;

    // метод загрузки типов присоединения
    void loadConnectionTypes();
    // метод загрузки уровней напряжения
    void loadVoltageLevel();

    // метод получения выбранных уровней напряжения
    QList<int> getSelectedVoltageIds() const;



};


#endif //INPUTCONNECTION_H

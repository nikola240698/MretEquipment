#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQueryModel>
// файл для создания запросов
#include <QSqlQuery>

// подключаем диалоговое окно ввода данных
#include "inputsubstation.h"

#include "inputconnection.h"
// подключаем класс базы данных
#include "database.h"
#include "connectionwindow.h"



QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(Database *inDb, QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    // выбор объекта в строке connectView
    void on_connectionsView_clicked(const QModelIndex &index);
    // слот выбора предприятия
    void onEnterpriseChanged(int index);
    // слот выбора подстанции
    void onSubstationChanged(int index);
    // слот нажатия на кнопку добавить
    void on_btnAddData_clicked();


    void on_btnAddConnection_clicked();


private:
    Ui::MainWindow *ui;
    //  создаем указатель на базу данных
    Database *db;
    // указатель на TableView для отображения БД
    QSqlQueryModel *connectionModel;
    //переменная для определения номера текущей строки
    int currentRow;


    // метод загрузки подстанций при выборе предприятия
    void loadSubstations(int enterpriseId);
    // метод загрузки присоединений
    void loadConnections(int substationId) const;
    //  метод двойного нажатия на присоединение из списка
    void onConnectionDoubleClicked(const QModelIndex &index);
};
#endif // MAINWINDOW_H

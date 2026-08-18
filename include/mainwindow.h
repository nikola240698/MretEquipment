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
#include "inputdata.h"
// подключаем класс базы данных
#include "database.h"



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
    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_tableView_clicked(const QModelIndex &index);

    void on_btnRefresh_clicked();

    void on_btnAddData_clicked();


private:
    Ui::MainWindow *ui;
    //  создаем указатель на базу данных
    Database *db;
    // указатель на TableView для отображения БД
    QSqlQueryModel *model;
    //переменная для определения номера текущей строки
    int currentRow;


    void updateTable() const;




};
#endif // MAINWINDOW_H

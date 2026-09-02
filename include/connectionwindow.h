#pragma once

#ifndef MRETEQUIPMENT_CONNECTIONWINDOW_H
#define MRETEQUIPMENT_CONNECTIONWINDOW_H

#include <QDialog>
#include <QSqlQueryModel>
#include <QSqlQuery>

#include "Database.h"

QT_BEGIN_NAMESPACE

namespace Ui
{
    class ConnectionWindow;
}

QT_END_NAMESPACE

class ConnectionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ConnectionWindow(int inConnectionId, Database *inDb, QWidget *parent = nullptr);

    ~ConnectionWindow() override;

private slots:

    // метод нажатия кнопки Close
    void on_btnClose_clicked();
    // метод нажатия кнопки Add
    void on_btnAddEquipment_clicked();
    // метод загрузки информации о присоединении
    void loadConnectionInfo();



private:

    Database *db;
    int connectionId;

    Ui::ConnectionWindow *ui;
    // указатель на QTableView в окне
    QSqlQueryModel *equipmentModel;
};



#endif //MRETEQUIPMENT_CONNECTIONWINDOW_H
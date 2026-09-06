//
// Created by RZAbook1 on 01.09.2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_ConnectionWindow.h" resolved

#include "connectionwindow.h"
#include "inputequipment.h"

#include <QMessageBox>
#include <QSqlError>

#include "ui_ConnectionWindow.h"



ConnectionWindow::ConnectionWindow(int inConnectionId, Database *inDb, QWidget *parent)
    : QDialog(parent), db(inDb), connectionId(inConnectionId), ui(new Ui::ConnectionWindow)
{

    ui->setupUi(this);

    equipmentModel = new QSqlQueryModel(this);

    loadConnectionInfo();

    // назначаем нашу модель в QTableView
    ui->equipmentView->setModel(equipmentModel);
    // указываем что выбор должен быть по всей строке
    ui->equipmentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // указываем, что выбор может быть только на одной строке
    ui->equipmentView->setSelectionMode(QAbstractItemView::SingleSelection);
    // запрещаем изменять данные в таблице
    ui->equipmentView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    loadEquipment();

}

ConnectionWindow::~ConnectionWindow()
{
    delete ui;
}

// метод нажатия кнопки Close
void ConnectionWindow::on_btnClose_clicked()
{
    reject();
}

// метод нажатия кнопки Add
void ConnectionWindow::on_btnAddEquipment_clicked()
{
    InputEquipment dialog(connectionId, db, this);

    if (dialog.exec() == QDialog::Accepted)
    {
        loadEquipment();
    }
}

// метод загрузки информации о присоединении
void ConnectionWindow::loadConnectionInfo()
{
    QSqlQuery query(db->getDatabase());

    query.prepare(
            "SELECT "
                "c.name AS connection_name, "
                "ct.name AS connection_type, "
                "("
                    "SELECT GROUP_CONCAT(voltage_level, '/') "
                    "FROM ("
                        "SELECT sv.voltage_level "
                        "FROM connection_voltages cv "
                        "JOIN substation_voltages sv "
                            "ON sv.id = cv.substation_voltage_id "
                        "WHERE cv.connection_id = c.id "
                        "ORDER BY CAST(sv.voltage_level AS INTEGER) DESC"
                    ")"
                ") || ' кВ' AS voltage "
            "FROM connections c "
            "JOIN connection_types ct "
                "ON ct.id = c.type_id "
            "WHERE c.id = 2;"
    );


    query.bindValue(":connectionId", connectionId);



    if (!query.exec())
    {
        QMessageBox::warning(this, "Error", query.lastError().text());
        return;
    }

    if (!query.next())
    {
        QMessageBox::warning(this, "Error", "Connection not found");
    }

    ui->lblConnectionName->setText(query.value("connection_name").toString());

    ui->lblConnectionType->setText(query.value("connection_type").toString());

    ui->lblConnectionVoltage->setText(query.value("voltage").toString());
}

void ConnectionWindow::loadEquipment()
{
    QSqlQuery query(db->getDatabase());

    query.prepare(
        "SELECT "
            "e.id, "
            "et.name AS equipment_type, "
            "e.name, "
            "e.serial_number, "
            "e.manufacturer, "
            "e.inventory_number, "
            "e.year_manufactured "
        "FROM equipment e "
        "JOIN equipment_types et "
            "ON et.id = e.equipment_type_id "
        "WHERE e.connection_id = :connectionId "
        "ORDER BY et.name, e.name;");

    query.bindValue("connectionId", connectionId);

    if (!query.exec())
    {
        qDebug() << "Error loading equipment: " << query.lastError().text();
        return;
    }


    equipmentModel->setQuery(std::move(query));


    ui->equipmentView->setModel(equipmentModel);

    ui->equipmentView->setColumnHidden(0, true);



    equipmentModel->setHeaderData(1, Qt::Horizontal, "Тип");
    equipmentModel->setHeaderData(2, Qt::Horizontal, "Наименование");
    equipmentModel->setHeaderData(3, Qt::Horizontal, "Заводской №");
    equipmentModel->setHeaderData(4, Qt::Horizontal, "Производитель");
    equipmentModel->setHeaderData(5, Qt::Horizontal, "Инвентарный №");
    equipmentModel->setHeaderData(6, Qt::Horizontal, "Год выпуска");

    ui->equipmentView->setColumnHidden(0, true);

    ui->equipmentView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->equipmentView->horizontalHeader()->setStretchLastSection(true);

}

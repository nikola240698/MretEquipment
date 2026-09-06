//
// Created by RZAbook1 on 02.09.2026.
//

// You may need to build the project (run Qt uic code generator) to get "ui_InputEquipment.h" resolved

#include "inputequipment.h"


#include <QLayout>
#include <QMessageBox>

#include "ui_InputEquipment.h"



InputEquipment::InputEquipment(
    int connectionId,
    Database *db,
    QWidget *parent
    )
        : QDialog(parent), ui(new Ui::InputEquipment), db(db), connectionId(connectionId)
{

    ui->setupUi(this);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    loadEquipmentTypes();
    loadParentEquipment();
}

InputEquipment::~InputEquipment()
{
    delete ui;
}

void InputEquipment::on_btnSave_clicked()
{
    const QString name = ui->ledtName->text().trimmed();

    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Input the equipment name.");
        return;
    }

    if (ui->typeBox->currentIndex() < 0)
    {
        QMessageBox::warning(this, "Error", "Select the types of the equipment.");
        return;
    }

    const int equipmentTypeId = ui->typeBox->currentData().toInt();

    const QVariant parentData = ui->parentEquipmentBox->currentData();

    QSqlQuery query(db->getDatabase());

    query.prepare(
        "INSERT INTO equipment ("
            "connection_id, "
            "equipment_type_id, "
            "parent_equipment_id, "
            "name, "
            "serial_number, "
            "manufacturer, "
            "inventory_number, "
            "year_manufactured, "
            "note"
        ") "
        "VALUES ("
            ":connectionId, "
            ":equipmentTypeId, "
            ":parentEquipmentId, "
            ":name, "
            ":serialNumber, "
            ":manufacturer, "
            ":inventoryNumber, "
            ":yearManufactured, "
            ":note"
        ");");

    query.bindValue(":connectionId", connectionId);

    query.bindValue(":equipmentTypeId", equipmentTypeId);

    if (parentData.isValid())
    {
        query.bindValue(":parentEquipmentId", parentData.toInt());
    } else
    {
        query.bindValue(":parentEquipmentId", QVariant());
    }

    query.bindValue(":name", name);

    query.bindValue(":serialNumber", ui->ledtSerialNumber->text().trimmed());

    query.bindValue("manufacturer", ui->ledtManufacturer->text().trimmed());

    query.bindValue(":inventoryNumber", ui->ledtInventoryNumber->text().trimmed());

    query.bindValue(":yearManufactured", ui->spbYearManufactured->value());

    query.bindValue(":note", ui->tedtNote->toPlainText().trimmed());

    if (!query.exec())
    {
        QMessageBox::warning(this, "Error", query.lastError().text());\
        return;
    }

    accept();

}

void InputEquipment::on_btnClose_clicked()
{
    reject();
}

void InputEquipment::loadParentEquipment()
{
    ui->parentEquipmentBox->clear();

    ui->parentEquipmentBox->addItem(
        "Нет", QVariant());

    QSqlQuery query(db->getDatabase());

    query.prepare(
        "SELECT "
            "e.id, "
            "e.name "
            "et.name AS type "
        "FROM equipment e "
        "JOIN equipment_types et "
            "ON et.id = e.equipment_type_id "
        "WHERE e.connection_id = :connectionId "
        "ORDER BY et.name, e.name;");

    query.bindValue(":connectionId", connectionId);

    if (!query.exec())
    {
        qDebug() << "Error loading parent equipment: " << query.lastError().text();
        return;
    }

    while (query.next())
    {
        const int id = query.value("id").toInt();

        const QString text = query.value("type").toString() + " - "
            + query.value("name").toString();

        ui->parentEquipmentBox->addItem(text, id);
    }

}

void InputEquipment::loadEquipmentTypes()
{
    ui->typeBox->clear();

    QSqlQuery query(db->getDatabase());

    if (!query.exec(
        "SELECT id, name "
        "FROM equipment_types "
        "ORDER BY name;"))
    {
        qDebug() << "Error loading equipment types: " << query.lastError().text();
        return;
    }

    while (query.next())
    {
        const int id = query.value("id").toInt();

        const QString name = query.value("name").toString();

        ui->typeBox->addItem(name, id);
    }

}

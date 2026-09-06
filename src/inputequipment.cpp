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

    // настраиваем окно
    ui->setupUi(this);

    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // загружаем типы оборудования и родительское оборудование
    loadEquipmentTypes();
    loadParentEquipment();
}

InputEquipment::~InputEquipment()
{
    delete ui;
}

// метод нажатия на кнопку сохранения
void InputEquipment::on_btnSave_clicked()
{
    // получаем название оборудования
    const QString name = ui->ledtName->text().trimmed();
    // проверяем, что имя введено
    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Input the equipment name.");
        return;
    }
    // проверяем, что выбран корректно тип оборудования
    if (ui->typeBox->currentIndex() < 0)
    {
        QMessageBox::warning(this, "Error", "Select the types of the equipment.");
        return;
    }
    // получаем id выбранного типа оборудования
    const int equipmentTypeId = ui->typeBox->currentData().toInt();
    // получаем id выбранного родителья-оборудования
    const QVariant parentData = ui->parentEquipmentBox->currentData();
    // создаем запрос
    QSqlQuery query(db->getDatabase());
    // подготавливаем запрос
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
    // вставляем переменные в запросе
    query.bindValue(":connectionId", connectionId);

    query.bindValue(":equipmentTypeId", equipmentTypeId);
    // проверяем, что родительское оборудование выбрано
    if (parentData.isValid())
    {
        query.bindValue(":parentEquipmentId", parentData.toInt());
    } else
    {
        query.bindValue(":parentEquipmentId", QVariant());
    }

    query.bindValue(":name", name);

    query.bindValue(":serialNumber", ui->ledtSerialNumber->text().trimmed());

    query.bindValue(":manufacturer", ui->ledtManufacturer->text().trimmed());

    query.bindValue(":inventoryNumber", ui->ledtInventoryNumber->text().trimmed());

    query.bindValue(":yearManufactured", ui->spbYearManufactured->value());

    query.bindValue(":note", ui->tedtNote->toPlainText().trimmed());
    // пробуем выполнить запрос
    if (!query.exec())
    {
        QMessageBox::warning(this, "Error", query.lastError().text());\
        return;
    }
    // принимаем изменения
    accept();
}

// метод нажатия кнопки отмены
void InputEquipment::on_btnClose_clicked()
{
    // отзываем все введенные данные
    reject();
}

// метод загрузки родителей-объектов
void InputEquipment::loadParentEquipment()
{
    // очищаем список
    ui->parentEquipmentBox->clear();
    // добавляем выбор отсутствия родителя
    ui->parentEquipmentBox->addItem(
        "Нет", QVariant());
    // создаем запрос
    QSqlQuery query(db->getDatabase());
    // подготавливаем запрос
    query.prepare(
        "SELECT "
            "e.id, "
            "e.name, "
            "et.name AS type "
        "FROM equipment e "
        "JOIN equipment_types et "
            "ON et.id = e.equipment_type_id "
        "WHERE e.connection_id = :connectionId "
        "ORDER BY et.name, e.name;");
    // биндим id присоединения
    query.bindValue(":connectionId", connectionId);
    // пробуем запустить запрос
    if (!query.exec())
    {
        qDebug() << "Error loading parent equipment: " << query.lastError().text();
        return;
    }
    // вставляем полученные данные в список
    while (query.next())
    {
        const int id = query.value("id").toInt();

        const QString text = query.value("type").toString() + " - "
            + query.value("name").toString();

        ui->parentEquipmentBox->addItem(text, id);
    }

}

// метод загрузки типов оборудования
void InputEquipment::loadEquipmentTypes()
{
    // очищаем список
    ui->typeBox->clear();
    // создаем запрос
    QSqlQuery query(db->getDatabase());
    // пробуем выполнить запрос
    if (!query.exec(
        "SELECT id, name "
        "FROM equipment_types "
        "ORDER BY name;"))
    {
        qDebug() << "Error loading equipment types: " << query.lastError().text();
        return;
    }

    // встаем значения в выпадающий список
    while (query.next())
    {
        const int id = query.value("id").toInt();

        const QString name = query.value("name").toString();

        ui->typeBox->addItem(name, id);
    }

}

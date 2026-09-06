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
    // настраиваем модель
    ui->setupUi(this);
    // создаем динамически нашу таблицу результата запроса
    equipmentModel = new QStandardItemModel(this);
    // загружаем данные присоединения
    loadConnectionInfo();

    // назначаем нашу модель в QTableView
    ui->equipmentView->setModel(equipmentModel);
    // указываем что выбор должен быть по всей строке
    ui->equipmentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    // указываем, что выбор может быть только на одной строке
    ui->equipmentView->setSelectionMode(QAbstractItemView::SingleSelection);
    // запрещаем изменять данные в таблице
    ui->equipmentView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->equipmentView->setStyleSheet(
        "QTreeView::item { padding: 4px 0px; }"
    );

    // загружаем существующее оборудование присоединения
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
    // создаем динамически наше окно
    InputEquipment dialog(connectionId, db, this);
    // проверяем, что оно удачно выполнено
    if (dialog.exec() == QDialog::Accepted)
    {
        // заново загружаем список оборудования
        loadEquipment();
    }
}

// метод загрузки информации о присоединении
void ConnectionWindow::loadConnectionInfo()
{
    // создаем запрос
    QSqlQuery query(db->getDatabase());
    // подготоавливаем запрос
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

    // биндим переменные
    query.bindValue(":connectionId", connectionId);
    // пробуем выполнить запрос
    if (!query.exec())
    {
        QMessageBox::warning(this, "Error", query.lastError().text());
        return;
    }
    // проверяем, что что-то найдено
    if (!query.next())
    {
        QMessageBox::warning(this, "Error", "Connection not found");
    }
    // вставляем найденные значения в форму
    ui->lblConnectionName->setText(query.value("connection_name").toString());

    ui->lblConnectionType->setText(query.value("connection_type").toString());

    ui->lblConnectionVoltage->setText(query.value("voltage").toString());
}

// метод загрузки оборудования выбранного присоединения
void ConnectionWindow::loadEquipment()
{
    equipmentModel->clear();

    equipmentModel->setHorizontalHeaderLabels(
        {
            "Оборудование",
            "Заводской №",
            "Производитель"
        }
    );



    // создаем запрос
    QSqlQuery query(db->getDatabase());
    // подготавливаем запрос
    query.prepare(
        "SELECT "
            "e.id, "
            "e.parent_equipment_id, "
            "e.name, "
            "e.serial_number, "
            "e.manufacturer, "
            "et.name AS equipment_type "
        "FROM equipment e "
        "JOIN equipment_types et "
            "ON et.id = e.equipment_type_id "
        "WHERE e.connection_id = :connectionId "
        "ORDER BY et.name, e.name;"
    );
    // биндим переменные
    query.bindValue(":connectionId", connectionId);
    // пробуем выполнить запрос
    if (!query.exec())
    {
        qDebug() << "Error loading equipment: " << query.lastError().text();
        return;
    }
    struct EquipmentItem
    {
        int id;
        QVariant parentId;

        QStandardItem *nameItem;
        QStandardItem *serialItem;
        QStandardItem *manufacturerItem;
    };

    QList<EquipmentItem> items;

    QHash<int, QStandardItem *> itemMap;

    while (query.next())
    {
        const int id = query.value("id").toInt();

        const QVariant parentId = query.value("parent_equipment_id");

        const QString type = query.value("equipment_type").toString();

        const QString name = query.value("name").toString();

        const QString serial = query.value("serial_number").toString();

        const QString manufacturer = query.value("manufacturer").toString();

        auto *nameItem = new QStandardItem(type + " - " + name);

        auto *serialItem = new QStandardItem(serial);

        auto *manufacturerItem = new QStandardItem(manufacturer);

        nameItem->setData(id, Qt::UserRole);

        itemMap.insert(id, nameItem);

        items.append(
            {
                id,
                parentId,
                nameItem,
                serialItem,
                manufacturerItem
            }
        );

    }

    for (const EquipmentItem &equipment : items)
    {
        QList<QStandardItem *> row;

        row
            << equipment.nameItem
            << equipment.serialItem
            << equipment.manufacturerItem;

        if (equipment.parentId.isNull() || !equipment.parentId.isValid())
        {
            equipmentModel->appendRow(row);
        } else
        {
            const int parentId = equipment.parentId.toInt();

            QStandardItem* parentItem = itemMap.value(parentId);

            if (parentItem)
            {
                parentItem->appendRow(row);
            } else
            {
                // если ссылка повреждена
                equipmentModel->appendRow(row);
            }
        }
    }

    ui->equipmentView->expandAll();

    ui->equipmentView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    ui->equipmentView->header()->setStretchLastSection(true);


}
















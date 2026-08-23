
#include "inputconnection.h"
#include "ui_inputconnection.h"


InputConnection::InputConnection(int inSubstationId, Database *inDb, QWidget *parent)
: QDialog(parent), ui(new Ui::InputConnection), db(inDb), substationId(inSubstationId)
{
    ui->setupUi(this);
}

InputConnection::~InputConnection()
{
    delete ui;
}

void InputConnection::loadConnectionTypes() const
{
    ui->typeBox->clear();

    QSqlQuery connectionQuery(db->getDatabase());

    if (!connectionQuery.exec(
        "SELECT id, name "
        "FROM connection_types "
        "ORDER BY name"))
    {
        qDebug() << connectionQuery.lastError().text();
        return;
    }

    while (connectionQuery.next())
    {
        ui->typeBox->addItem(
            connectionQuery.value("name").toString(),
            connectionQuery.value("id").toInt()
        );
    }
}

void InputConnection::loadVoltageLevel() const
{
    ui->voltageBox->clear();

    QSqlQuery voltageQuery(db->getDatabase());

    voltageQuery.prepare (
        "SELECT voltage_level "
        "FROM substation_voltages "
        "WHERE substation_id = :substationId "
        "ORDER BY CAST(voltage_level AS INTEGER) DESC");

    qDebug() << "SubstationId" << substationId;
    voltageQuery.bindValue(":substationId", substationId);

    if (!voltageQuery.exec())
    {
        qDebug() << voltageQuery.lastError().text();
        return;
    }

    while (voltageQuery.next())
    {
        int voltage = voltageQuery.value("voltage_level").toInt();

        ui->voltageBox->addItem(QString::number(voltage) + " кВ", voltage);
    }
}


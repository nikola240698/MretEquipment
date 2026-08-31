
#include "inputconnection.h"

#include <QCheckBox>

#include "ui_inputconnection.h"

// конструктор класса
InputConnection::InputConnection(
    int inSubstationId,
    Database *inDb,
    QWidget *parent
    )
        : QDialog(parent),
        ui(new Ui::InputConnection),
        db(inDb),
        substationId(inSubstationId)
{
    ui->setupUi(this);

    // станавливаем возможность автоматического изменения размера окна при изменении параметров
    layout()->setSizeConstraint(QLayout::SetFixedSize);

    // название окна
    setWindowTitle("Adding connection");

    // по умолчанию скрываем оба элемента
    // после выбора типа необходимы отобразится
    ui->voltageBox->hide();
    ui->voltageGroupBox->hide();

    // вызываем методы отображения данных по выбранной ПС
    // порядок из очень важен, сначала нам необходимы напряжения
    loadVoltageLevel();
    loadConnectionTypes();
}

// деструктор класса
InputConnection::~InputConnection()
{
    delete ui;
}

// слот выбора типа оборудования
void InputConnection::on_typeBox_currentIndexChanged(int index)
{
    // проверяем на правильный индекс
    if (index < 0)
    {
        return;
    }

    // проверяем много или один уровень напряжения у присоединения
    const bool multipleVoltages = ui->typeBox->itemData(index, Qt::UserRole + 1).toBool();

    // изменяем показываемые поля при выборе одного или другого типа присоединения
    ui->voltageBox->setVisible(!multipleVoltages);
    ui->voltageGroupBox->setVisible(multipleVoltages);
}

// метод загрузки типов присоединения
void InputConnection::loadConnectionTypes()
{
    // очищаем список
    ui->typeBox->clear();
    // создаем запрос
    QSqlQuery connectionQuery(db->getDatabase());

    // подготавливаем запрос
    connectionQuery.prepare(
        // выводим все данные из таблицы
        "SELECT id, name, multiple_voltages "
        "FROM connection_types "
        "ORDER BY name;");

    // проверяем, что запрос можно выполнить
    if (!connectionQuery.exec())
    {
        // выводим сообщение при ошибке открытия
        qDebug() << "Error loading connection types: " << connectionQuery.lastError().text();
        return;
    }

    // заполняем полученные данные в необходимые объекты
    while (connectionQuery.next())
    {
        // получаем id типа оборудования
        const int id = connectionQuery.value("id").toInt();
        // получаем название типа оборудования
        const QString name = connectionQuery.value("name").toString();
        // получаем переменную количества имеющихся уровней напряжения
        const bool multipleVoltages = connectionQuery.value("multiple_voltages").toBool();

        // добавляем название в бокс
        ui->typeBox->addItem(name);

        const int index = ui->typeBox->count() - 1;

        // получаем id типа
        ui->typeBox->setItemData(index, id, Qt::UserRole);

        // проверяем может ли иметь несколько уровней напряжения
        ui->typeBox->setItemData(index, multipleVoltages, Qt::UserRole + 1);
    }
    // обновляем интерфейс
    if (ui->typeBox->count() > 0)
    {
        on_typeBox_currentIndexChanged(ui->typeBox->currentIndex());
    }
}

// метод загрузки уровней напряжения
void InputConnection::loadVoltageLevel()
{
    // очищаем список
    ui->voltageBox->clear();

    // удаляем старые чекбоксы если они существовали
    for (QCheckBox *checkBox : voltageCheckBoxes)
    {
        delete checkBox;
    }
    voltageCheckBoxes.clear();

    // создаем запрос
    QSqlQuery voltageQuery(db->getDatabase());
    // подготавливаем запрос
    voltageQuery.prepare (
        "SELECT id, voltage_level "
        "FROM substation_voltages "
        "WHERE substation_id = :substationId "
        "ORDER BY CAST(voltage_level AS INTEGER) DESC;");
    // выводим информацию об выбранной ПС
    qDebug() << "SubstationId" << substationId;
    // заполняем данные в запрос
    voltageQuery.bindValue(":substationId", substationId);
    // проверяем возможность выполнения запроса
    if (!voltageQuery.exec())
    {
        // выводим ошибку
        qDebug() << "Error loading voltage levels: " << voltageQuery.lastError().text();
        return;
    }

    // заполняем выпадающий список данными из запроса
    while (voltageQuery.next())
    {
        // получаем id оборудования
        const int voltageId = voltageQuery.value("id").toInt();
        // получаем уровни напряжения
        const int voltage = voltageQuery.value("voltage_level").toInt();
        // переводим уровень напряжение в текст
        const QString text = QString::number(voltage) + " кВ";

        // для списка напряжений
        ui->voltageBox->addItem(text, voltageId);

        // для создания checkBox'ов
        auto *checkBox = new QCheckBox(text, ui->voltageGroupBox);

        // получаем id строки
        checkBox->setProperty("voltageId", voltageId);

        // добавляем виджет
        ui->voltageLayout->addWidget(checkBox);
        // добавляем чекбоксы
        voltageCheckBoxes.append(checkBox);
    }
}

// метод получения выбранного напряжения
QList<int> InputConnection::getSelectedVoltageIds() const
{
    // создаем список id напряжений
    QList<int> voltageIds;
    // переменная текущего индекса типа присоединения
    const int index = ui->typeBox->currentIndex();
    // проверяем на правильный индекс
    if (index < 0)
        return voltageIds;
    // переменная выбора количества уровней напряжения
    const bool multipleVoltages = ui->typeBox->itemData(index, Qt::UserRole + 1).toBool();

    // если одно напряжения
    if (!multipleVoltages)
    {
        if (ui->voltageBox->currentIndex() >= 0)
        {
            voltageIds.append(ui->voltageBox->currentData().toInt());
        }
        return voltageIds;
    }

    // несколько напряжений
    for (QCheckBox *checkBox : voltageCheckBoxes)
    {
        if (checkBox->isChecked())
        {
            voltageIds.append(checkBox->property("voltageId").toInt());
        }
    }
    return voltageIds;
}

// слот нажатия на кнопку отмены
void InputConnection::on_btnClose_clicked()
{
    // отклоняем работу диалогового окна
    this->reject();
}

// слот нажатия кнопки сохранения
void InputConnection::on_btnSave_clicked()
{
    // проверка названия
    const QString name = ui->ledtName->text().trimmed();
    if (name.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Input the connection name.");
        return;
    }

    // проверка типа
    if (ui->typeBox->currentIndex() < 0)
    {
        QMessageBox::warning(this, "Error", "Select the type of the connection");
        return;
    }

    const int typeId = ui->typeBox->currentData(Qt::UserRole).toInt();
    const bool multipleVoltages = ui->typeBox->currentData(Qt::UserRole + 1).toBool();

    // получаем напряжение
    const QList<int> voltagesIds = getSelectedVoltageIds();

    // обычное присоединение
    if (!multipleVoltages && voltagesIds.size() != 1)
    {
        QMessageBox::warning(this, "Error", "Select voltage level.");
        return;
    }

    // многоуровненвое напряжение
    if (multipleVoltages && voltagesIds.size() < 2)
    {
        QMessageBox::warning(this, "Error", "Select minimum two voltage level");
        return;
    }

    // начинаем транзакцию
    if (!db->transaction())
    {
        QMessageBox::critical(this, "Error", db->lastError());
        return;
    }

    // добавляем присоединение
    QSqlQuery connectionQuery(db->getDatabase());

    connectionQuery.prepare(
        "INSERT INTO connections "
        "(substation_id, type_id, name) "
        "VALUES "
        "(:substationId, :typeId, :name);"
        );

    connectionQuery.bindValue(":substationId", substationId);

    connectionQuery.bindValue(":typeId", typeId);

    connectionQuery.bindValue(":name", name);

    if (!connectionQuery.exec())
    {
        db->rollback();

        QMessageBox::critical(this, "Adding connection error",
            connectionQuery.lastError().text());
        return;
    }

    const int connectionId = connectionQuery.lastInsertId().toInt();

    // добавляем напряжение

    QSqlQuery voltageQuery(db->getDatabase());

    voltageQuery.prepare(
        "INSERT INTO connection_voltages "
        "(connection_id, substation_voltage_id) "
        "VALUES "
        "(:connectionId, :voltageId);"
        );

    for (const int voltageId : voltagesIds)
    {
        voltageQuery.bindValue(":connectionId", connectionId);

        voltageQuery.bindValue(":voltageId", voltageId);

        if (!voltageQuery.exec())
        {
            db->rollback();

            QMessageBox::critical(this, "Adding voltage error",
                voltageQuery.lastError().text());
            return;
        }
    }

    // подтверждаем транзакцию
    if (!db->commit())
    {
        db->rollback();

        QMessageBox::critical(this, "Error", db->lastError());
        return;
    }

    accept();
}




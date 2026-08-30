
#include "inputconnection.h"
#include "ui_inputconnection.h"

// конструктор класса
InputConnection::InputConnection(int inSubstationId, Database *inDb, QWidget *parent)
: QDialog(parent), ui(new Ui::InputConnection), db(inDb), substationId(inSubstationId)
{
    ui->setupUi(this);

    // вызываем методы отображения данных по выбранной ПС
    loadConnectionTypes();
    loadVoltageLevel();
}

// деструктор класса
InputConnection::~InputConnection()
{
    delete ui;
}

// метод загрузки типов присоединения
void InputConnection::loadConnectionTypes() const
{
    // очищаем список
    ui->typeBox->clear();
    // создаем запрос
    QSqlQuery connectionQuery(db->getDatabase());
    // проверяемЮ что запрос можно выполнить
    if (!connectionQuery.exec(
        // выводим все данные из таблицы
        "SELECT id, name "
        "FROM connection_types "
        "ORDER BY name;"))
    {
        // выводим сообщение при ошибке открытия
        qDebug() << "In connection types query: " << connectionQuery.lastError().text();
        return;
    }

    // заполняем выпадающий список полученными данными
    while (connectionQuery.next())
    {
        // добавляем и имя и Id
        ui->typeBox->addItem(
            connectionQuery.value("name").toString(),
            connectionQuery.value("id").toInt()
        );
    }
}

// метод загрузки уровней напряжения
void InputConnection::loadVoltageLevel() const
{
    // очищаем список
    ui->voltageBox->clear();
    // создаем запрос
    QSqlQuery voltageQuery(db->getDatabase());
    // подготавливаем запрос
    voltageQuery.prepare (
        "SELECT voltage_level "
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
        qDebug() << "In voltage level DB: " << voltageQuery.lastError().text();
        return;
    }

    // заполняем выпадающий список данными из запроса
    while (voltageQuery.next())
    {
        // получаем значения напряжения в целочисленном типе
        int voltage = voltageQuery.value("voltage_level").toInt();
        // вставляем данные прибавляя приставку в конце
        ui->voltageBox->addItem(QString::number(voltage) + " кВ", voltage);
    }
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
    // проверяем, что имя введено
    if (ui->ledtName->text().trimmed().isEmpty())
    {
        QMessageBox::warning(this, "Error name", "Input name of the connection");
        return;
    }
    // получаем id типа присоединения
    int typeId = ui->typeBox->currentData().toInt();
    // получаем id уровня напряжения
    int voltage = ui->voltageBox->currentData().toInt();
    // получем название присоединения
    QString name = ui->ledtName->text().trimmed();
    // создаем запрос
    QSqlQuery query(db->getDatabase());
    // подготавливаем запрос
    query.prepare("INSERT INTO connections "
                  "(substation_id, type_id, name, voltage_level) "
                  "VALUES "
                  "(:substationId, :typeId, :name, :voltage);");
    // вставляем данные в запрос
    query.bindValue(":substationId", substationId);
    query.bindValue(":typeId", typeId);
    query.bindValue(":name", name);
    query.bindValue(":voltage", voltage);
    // проверяем его возможность исполнения
    if (!query.exec())
    {
        QMessageBox::critical(this, "Error save", query.lastError().text());
        return;
    }
    // закрываем окно с применением данных
    accept();
}




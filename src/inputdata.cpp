#include "inputdata.h"
#include "ui_inputdata.h"

InputData::InputData(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::InputData)
{
    ui->setupUi(this);

    // изменяем название диалогового окна
    this->setWindowTitle("Input new data");

    // фиксируем размер окна
    this->setFixedSize(this->size());

    //  очищаем поля ввода данных
    ui->ledtName->clear();
    ui->ledtVoltage->clear();

    //фокусируемся на первом LineEdit
    ui->ledtName->setFocus();

    // загружаем данные из БД для списка предприятий
    loadMaintanceName();

    // Связываем кнопку "Save" со слотом принятиe результата диалога (QDialog::accept)
    connect(ui->btnSave, &QPushButton::clicked, this, &InputData::on_btnSave_clicked);
}

InputData::~InputData()
{
    delete ui;
}



// получаем текст из ledtName
QString InputData::getName() const
{
    return ui->ledtName->text();
}

// получаем текст из ledtVoltage
QString InputData::getVoltage() const
{
    return ui->ledtVoltage->text();
}

// получаем Id предприятия из списка
int InputData::getMaintanceId() const
{
    return ui->maintanceBox->currentData().toInt();
}


// слот обработки нажатия кнопки "Close"
void InputData::on_btnClose_clicked()
{
    // закрываем окно
    this->close();
}

// слот обработки нажатия кнопки "Save"
void InputData::on_btnSave_clicked()
{
    // флаг проверки введенных данных
    bool isOk = true;

    // проверяем втору строку
    if (getName() == "")
    {
        isOk = false;
        ui->lblName->setText("Not be null");
    }
    // проверяем третью строку
    if (getVoltage() == "")
    {
        isOk = false;
        ui->lblVoltage->setText("Not be null");
    }
    // если всё хорошо, то закрываем успешно окно
    if (isOk)
    {
        accept();
    }



}

// метод заполнения MaintanceBox используя запрос БД
void InputData::loadMaintanceName()
{
    // очищаем наш список
    ui->maintanceBox->clear();

    // оздаем запрос
    QSqlQuery maintanceQuery;
    // проверяем, что запрос успешно выполняется
    if (!maintanceQuery.exec("SELECT id, name FROM MAINTANCE ORDER BY id;"))
    {
        QMessageBox::critical(this, "Error", "There are not the list of maintnace:\n"
                                                 + maintanceQuery.lastError().text());
        return;
    }

    // начинаем читать ответ на запрос поочередно
    while (maintanceQuery.next())
    {
        // достаем id строки
        int id = maintanceQuery.value(0).toInt();
        // достаем название предприятия
        QString name = maintanceQuery.value(1).toString();
        // доавляем все найденные записи в выпадающий список
        ui->maintanceBox->addItem(name, id);
    }
}
















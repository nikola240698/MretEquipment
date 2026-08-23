#include "inputsubstation.h"
#include "ui_inputsubstation.h"


InputSubstation::InputSubstation(QWidget *parent) : QDialog(parent), ui(new Ui::InputSubstation)
{
    ui->setupUi(this);

    // изменяем название диалогового окна
    this->setWindowTitle("Input new data");

    // фиксируем размер окна
    this->setFixedSize(this->size());

    //  очищаем поля ввода данных
    ui->ledtName->clear();


    //фокусируемся на первом LineEdit
    ui->ledtName->setFocus();

    // загружаем данные из БД для списка предприятий
    loadEnterpriseName();



}

InputSubstation::~InputSubstation()
{
    delete ui;
}
// получаем текст из ledtName
QString InputSubstation::getName() const
{
    return ui->ledtName->text();
}

// получаем Id предприятия из списка
int InputSubstation::getEnterpriseId() const
{
    return ui->enterpriseBox->currentData().toInt();
}

// метод получения уровней напряжения из чекбоксов
QList<int> InputSubstation::getVoltage() const
{
    // создаем список
    QList<int> voltages;
    // проверяем
    // каждый чекбокс
    if (ui->chbx220->isChecked())
        voltages.append(220);
    if (ui->chbx110->isChecked())
        voltages.append(110);
    if (ui->chbx35->isChecked())
        voltages.append(35);
    if (ui->chbx10->isChecked())
        voltages.append(10);
    if (ui->chbx6->isChecked())
        voltages.append(6);

    return voltages;
}

// слот обработки нажатия кнопки "Close"
void InputSubstation::on_btnClose_clicked()
{
    // закрываем окно
    this->close();
}

// слот обработки нажатия кнопки "Save"
void InputSubstation::on_btnSave_clicked()
{
    // флаг проверки введенных данных
    bool isOk = true;

    // проверяем втору строку
    if (getName() == "")
    {
        isOk = false;
        ui->lblName->setText("Not be null");
    }

    // получаем список выбранных уровней напряжения
    QList<int> voltages = getVoltage();
    // проверяем что выбрано минимум два уровня напряжения
    if (voltages.size() < 2)
    {
        isOk = false;

        QMessageBox::warning(this, "Voltage level", "Should be checked minimum 2 level of voltage");
    }
    // если всё хорошо, то закрываем успешно окно
    if (isOk)
    {
        accept();
    }
}

// метод заполнения enterpriseBox используя запрос БД
void InputSubstation::loadEnterpriseName()
{
    // очищаем наш список
    ui->enterpriseBox->clear();

    // создаем запрос
    QSqlQuery enterpriseQuery;
    // проверяем, что запрос успешно выполняется
    if (!enterpriseQuery.exec("SELECT id, name FROM enterprise ORDER BY id;"))
    {
        QMessageBox::critical(this, "Error", "There are not the list of enterprises:\n"
                                                 + enterpriseQuery.lastError().text());
        return;
    }

    // начинаем читать ответ на запрос поочередно
    while (enterpriseQuery.next())
    {
        // достаем id строки
        int id = enterpriseQuery.value(0).toInt();
        // достаем название предприятия
        QString name = enterpriseQuery.value(1).toString();
        // доавляем все найденные записи в выпадающий список
        ui->enterpriseBox->addItem(name, id);
    }
}
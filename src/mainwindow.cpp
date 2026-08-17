#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>

MainWindow::MainWindow(Database *inDb, QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), db(inDb)
{
    ui->setupUi(this);





    if (db->isOpen())
    {
        // выводим сообщение в статусБар
        ui->statusbar->showMessage("Successful connect to DB: " + db->databasePath());

        // создаем динамическую модель БД, указав родителя
        model = new QSqlQueryModel(this);

        // обновляем данные таблицы
        updateTable();

        // указываем куда будем выводить данные
        ui->tableView->setModel(model);

        // выделяем всю строку, а не отдельно ячейку при нажатии мышью
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        //запрещаем редактирование
        ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

        // разрешаем сортировку по столбцам
        ui->tableView->setSortingEnabled(true);

        // изменяем размер столбцов таблицы
        ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else
    {
        ui->statusbar->showMessage("Database doesn't open");
    }









}

MainWindow::~MainWindow()
{

    delete ui;
}

// слот(метод) нажатия кнопки "Add"
void MainWindow::on_btnAdd_clicked()
{
    // осуществляем добавление строки передавая количество строк
    model->insertRow(model->rowCount());
}

// слот(метод) нажатия кнопки "Remove"
void MainWindow::on_btnRemove_clicked()
{
    // удаляем строку по индексу
    model->removeRow(currentRow);
    // снова вызываем запрос для обновления данных
    updateTable();
}

// слот(метод) нажатия на TableView для опередедлния индкса текущей строки
void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    // сохраняем индекс строки
    currentRow = index.row();

}

// слот(метод) нажатия кнопки "Refresh"
void MainWindow::on_btnRefresh_clicked()
{
    // запрос на прочтение новых данных
    updateTable();
    // выделяем последний раз вбранную строку
    ui->tableView->selectRow(currentRow);
}

// слот(метод) нажатия кнопки "Add Data"
void MainWindow::on_btnAddData_clicked()
{
    //создаем объект диалогового окна
    InputData dialog(this);

    // делаем диалоговое окно модальным, чтобы не позволяло взаимодейтсвоавть с основным окном
    dialog.setModal(true);
    // вызываем диалоговое окно как исполняемое с проверкой успшного выполнения
    if (dialog.exec() == QDialog::Accepted)
    {
        // получаем id из списка предприятий
        int maintanceId = dialog.getMaintanceId();
        // получаем данные из строк
        QString name = dialog.getName();
        QString voltage = dialog.getVoltage();

        // создаем динамический запрос
        QSqlQuery *query = new QSqlQuery();

        // подготавливаем запрос с заполнениями по полям
        query->prepare("INSERT INTO SUBSTATIONS (maintance_id, name, voltage_level) VALUES (:f1, :f2, :f3)");

        // Привязываем значения к меткам (позиционная привязка или по именам)
        query->bindValue(":f1", maintanceId);
        query->bindValue(":f2", name);
        query->bindValue(":f3", voltage);

        // Выполняем запрос
        if (query->exec()) {
            ui->statusbar->showMessage("Data added successfully");
        } else {
            QMessageBox::critical(this, "Error", "Error adding record:\n" + query->lastError().text());
        }

        query->finish();

        updateTable();
    }
}

// метод вызова вывода необходимого содержания БД
void MainWindow::updateTable() const
{
    QSqlQuery query;


    query.prepare("SELECT m.name, s.name, s.voltage_level "
                       "FROM SUBSTATIONS s "
                       "LEFT JOIN MAINTANCE m "
                       "ON s.maintance_id = m.id;");

    // Выполняем запрос
    if (query.exec()) {
        qDebug() << "Database read succesfully";

    } else {
        qDebug() << "Error reading record:\n" + query.lastError().text();
        return;
    }

    // передаем в модель получившуюся таблицу
    model->setQuery(std::move(query));

    // меняем названия столбцов
    model->setHeaderData(
        0, Qt::Horizontal, "Предприятие");
    model->setHeaderData(
        1, Qt::Horizontal, "Название ПС");
    model->setHeaderData(
        2, Qt::Horizontal, "Напряжение");




}


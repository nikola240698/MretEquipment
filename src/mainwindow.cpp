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
    if (dialog.exec() != QDialog::Accepted)
        return;

    // Получаем данные из диалогового окна
    int maintanceId = dialog.getMaintanceId();
    QString name = dialog.getName();

    QList<int> voltages = dialog.getVoltage();

    // начинаем транзакцию
    if (!db->transaction())
    {
        qDebug() << "Transaction doesn't start: " << db->lastError();
        return;
    }

    // добавляем подстанцию
    QSqlQuery substationQuery(db->getDatabase());

    substationQuery.prepare("INSERT INTO substations "
                            "(maintance_id, name) "
                            "VALUES (:maintance_id, :name)");

    substationQuery.bindValue(":maintance_id", maintanceId);
    substationQuery.bindValue(":name", name);


    if (!substationQuery.exec())
    {
        qDebug() << "Error adding substation: " << substationQuery.lastError().text();

        db->rollback();
        return;
    }

    // Получаем ID подстанции
    int substationId = substationQuery.lastInsertId().toInt();

    // запрос на добавление напряжений
    QSqlQuery voltageQuery(db->getDatabase());

    voltageQuery.prepare("INSERT INTO substation_voltages "
                         "(substation_id, voltage_level) "
                         "VALUES (:substation_id, :voltage)");

    for (int voltage : voltages)
    {
        voltageQuery.bindValue(":substation_id", substationId);

        voltageQuery.bindValue(":voltage", voltage);

        if (!voltageQuery.exec())
        {
            qDebug() << "Error voltage adding: " << voltageQuery.lastError().text();

            db->rollback();
            return;
        }
    }

    // фиксируем транзакцию
    if (!db->commit())
    {
        qDebug() << "Commit error: " << db->lastError();

        db->rollback();
        return;
    }

    updateTable();
}

// метод вызова вывода необходимого содержания БД
void MainWindow::updateTable() const
{
    QSqlQuery query;

    query.prepare("SELECT m.name, s.name, ("
	                "SELECT GROUP_CONCAT(voltage_level, '/') "
	               "FROM ("
		                "SELECT voltage_level "
		                "FROM substation_voltages sv "
		                "WHERE substation_id = s.id "
		                "ORDER BY voltage_level DESC "
		                ")"
	                ") || ' кВ' AS voltages "
	                "FROM substations s "
	                "LEFT JOIN maintance m "
	                "ON s.maintance_id = m.id "
	                "ORDER BY s.name;"
                  );

    // Выполняем запрос
    if (query.exec()) {
        qDebug() << "Database read successfully";

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


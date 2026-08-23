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
        connectionModel = new QSqlQueryModel(this);
        // обновляем данные таблицы
        updateTable();
        // указываем куда будем выводить данные
        ui->connectionsView->setModel(connectionModel);
        // выделяем всю строку, а не отдельно ячейку при нажатии мышью
        ui->connectionsView->setSelectionBehavior(QAbstractItemView::SelectRows);
        //запрещаем редактирование
        ui->connectionsView->setSelectionBehavior(QAbstractItemView::SelectRows);
        // разрешаем сортировку по столбцам
        ui->connectionsView->setSortingEnabled(true);
        // изменяем размер столбцов таблицы
        ui->connectionsView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    } else
    {
        ui->statusbar->showMessage("Database doesn't open");
    }

    // выводим список предприятий в QComboBox
    // очищаем наш список
    ui->enterpriseBox->clear();
    // выводим надпись для просьбы выбрать предприятие
    ui->enterpriseBox->addItem("Выберите предприятие...", QVariant());
    // создаем запрос
    QSqlQuery enterpriseQuery(db->getDatabase());
    // проверяем, что запрос успешно выполняется
    if (!enterpriseQuery.exec("SELECT id, name FROM enterprise ORDER BY id;"))
    {
        QMessageBox::critical(this, "Error", "There are not the list of enterprise:\n"
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
        // добавляем все найденные записи в выпадающий список
        ui->enterpriseBox->addItem(name, id);
    }

    // подключаем слот выбора предприятия
    connect(ui->enterpriseBox, &QComboBox::currentIndexChanged, this, &MainWindow::onEnterpriseChanged);
    // подключем слот выбора подстанции
    connect(ui->substationBox, &QComboBox::currentIndexChanged, this, &MainWindow::onSubstationChanged);
    // етод двойного нажатия на строку в таблице
    connect(ui->connectionsView, &QTableView::doubleClicked, this, &MainWindow::onConnectionDoubleClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}




// слот(метод) нажатия на TableView для опередедлния индкса текущей строки
void MainWindow::on_connectionsView_clicked(const QModelIndex &index)
{
    // сохраняем индекс строки
    currentRow = index.row();

}

void MainWindow::onEnterpriseChanged(int index)
{
    // проверяем, что выбрали предприятие а не надпись выберите предприятие
    if (!ui->enterpriseBox->itemData(index).isValid())
        return;
    // получаем id предприятия из списка
    int enterpriseId = ui->enterpriseBox->currentData().toInt();
    qDebug() << enterpriseId;
    // загружаем подстанциии выбранного предприятия
    loadSubstations(enterpriseId);
    // проверяем, что в списке ещё есть строка "Выберите предприятие..."
    if (!ui->enterpriseBox->itemData(0).isValid())
    {
        // блокируем смещение id списка для контроля правильности ввода индексов
        ui->enterpriseBox->blockSignals(true);
        // удаляем не нужную строку просьбы выбора
        ui->enterpriseBox->removeItem(0);
        // нимаем блокировку
        ui->enterpriseBox->blockSignals(false);
    }
}

// слот при выбор подстанции
void MainWindow::onSubstationChanged(int index)
{
    // олучаем индекс списка подстанций
    QVariant data = ui->substationBox->itemData(index);
    // проверяем что выбрана именно подстанция, а не другое что-то
    if (!data.isValid())
        return;
    // получаем индекс самой подстанции
    int substationId = data.toInt();
    // загружаем присоединения подстанции
    loadConnections(substationId);
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
    int eterpriseId = dialog.getEnterpriseId();
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
                            "(enterprise_id, name) "
                            "VALUES (:enterprise_id, :name)");

    substationQuery.bindValue(":enterprise_id", eterpriseId);
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
    connectionModel->setQuery(std::move(query));

    // меняем названия столбцов
    connectionModel->setHeaderData(
        0, Qt::Horizontal, "Предприятие");
    connectionModel->setHeaderData(
        1, Qt::Horizontal, "Название ПС");
    connectionModel->setHeaderData(
        2, Qt::Horizontal, "Напряжение");




}

// метод загрузки списка подстанций
void MainWindow::loadSubstations(int enterpriseId)
{
    // очищаем существующий список
    ui->substationBox->clear();
    // создаем запрос
    QSqlQuery substationQuery(db->getDatabase());
    // подготавливаем запрос
    substationQuery.prepare("SELECT "
                                    "s.id, "
                                    "s.name || ' ' || ( "
                                        "SELECT GROUP_CONCAT(voltage_level, '/') "
                                        "FROM ( "
                                            "SELECT voltage_level "
                                            "FROM substation_voltages "
                                            "WHERE substation_id = s.id "
                                            "ORDER BY CAST(voltage_level AS INTEGER) DESC "
                                        ") "
                                    ") || ' кВ' AS name "
                                "FROM substations s "
                                "WHERE s.enterprise_id = :enterpriseId "
                                "ORDER BY s.name"
                                );
    // заполняем запрос данными
    substationQuery.bindValue(":enterpriseId", enterpriseId);
    // пробуем выполнить запрос
    if (!substationQuery.exec())
    {
        qDebug() << substationQuery.lastError().text();
        return;
    }
    // добавляем надпись просьбы выбора в список
    ui->substationBox->addItem("Выберите подстанцию...", QVariant());
    // заполняем список нашими подстанциями
    while (substationQuery.next())
    {
        int id = substationQuery.value("id").toInt();
        QString name = substationQuery.value("name").toString();
        ui->substationBox->addItem(name, id);
    }
}

// метод вывода присоединений подстанции
void MainWindow::loadConnections(int substationId) const
{
    // создаем запрос
    QSqlQuery connectionQuery(db->getDatabase());
    // подготавливаем запрос
    connectionQuery.prepare("SELECT "
                            "c.id, ct.name AS type, c.name, c.voltage_level "
                            "FROM connections c "
                            "LEFT JOIN connection_types ct "
                            "ON c.type_id = ct.id "
                            "WHERE c.substation_id = :substationId "
                            "ORDER BY c.voltage_level DESC, c.name;"
                            );
    // вставляем данные в запрос
    connectionQuery.bindValue(":substationId", substationId);
    // пропбуем исполнить запрос
    if (!connectionQuery.exec())
    {
        qDebug() << "Connection DB error:";
        qDebug() << connectionQuery.lastError().text();
        return;
    }
    // вставляем резульат в окно
    connectionModel->setQuery(std::move(connectionQuery));
    // задаем названия столбцам
    connectionModel->setHeaderData(1, Qt::Horizontal, "Тип");
    connectionModel->setHeaderData(2, Qt::Horizontal, "Наименоваание");
    connectionModel->setHeaderData(3, Qt::Horizontal, "U, кВ");
    // скрываем столбец id
    ui->connectionsView->setColumnHidden(0, true);
}
// метод двойного нажатия на присоединение из списка
void MainWindow::onConnectionDoubleClicked(const QModelIndex &index) const
{
    // получаем индекс строки
    int row = index.row();
    // получем индекс присоединения из нашего списка в прятанном столбце
    int connectionId = connectionModel->data(connectionModel->index(row, 0)).toInt();
    // выводим ссобщени о выбранном присоединении
    qDebug() << "Opening connection: " << connectionId;
}


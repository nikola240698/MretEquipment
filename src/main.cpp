#include "mainwindow.h"

#include <QApplication>

#include "database.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Открываем БД
    Database db;

    if (!db.open())
    {
        QMessageBox::critical(nullptr, "Error", "Failed to open Database");

        return -1;
    }

    MainWindow w(&db);
    w.show();
    return QApplication::exec();
}

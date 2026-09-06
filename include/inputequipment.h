//
// Created by RZAbook1 on 02.09.2026.
//

#ifndef INPUTEQUIPMENT_H
#define INPUTEQUIPMENT_H

#include <QDialog>

#include "database.h"


QT_BEGIN_NAMESPACE

namespace Ui
{
    class InputEquipment;
}

QT_END_NAMESPACE

class InputEquipment : public QDialog
{
    Q_OBJECT

public:
    explicit InputEquipment(
        int connectionId,
        Database* db,
        QWidget *parent = nullptr
        );

    ~InputEquipment() override;

private slots:

    void on_btnSave_clicked();

    void on_btnClose_clicked();

private:
    Ui::InputEquipment *ui;

    Database* db;
    int connectionId;

    void loadEquipmentTypes();
    void loadParentEquipment();
};


#endif //INPUTEQUIPMENT_H
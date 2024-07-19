#ifndef ADDDORMITORYDIALOG_H
#define ADDDORMITORYDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

class AddDormitoryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddDormitoryDialog(QWidget *parent = nullptr, int nextDormNumber = 1);

    QString getBuilding() const;
    QString getFloor() const;
    QString getDormNumber() const;
    QString getType() const;
    QString getBedId() const;

private:
    QLineEdit *buildingEdit;
    QLineEdit *floorEdit;
    QLineEdit *dormNumberEdit;
    QLineEdit *typeEdit;
    QLineEdit *bedIdEdit;
};

#endif // ADDDORMITORYDIALOG_H



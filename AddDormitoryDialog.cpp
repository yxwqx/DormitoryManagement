#include "AddDormitoryDialog.h"

AddDormitoryDialog::AddDormitoryDialog(QWidget *parent, int nextDormNumber) : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("Ìí¼ÓËÞÉá"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    buildingEdit = new QLineEdit(this);
    floorEdit = new QLineEdit(this);
    dormNumberEdit = new QLineEdit(this);
    typeEdit = new QLineEdit(this);
    bedIdEdit = new QLineEdit(this);

    formLayout->addRow(QString::fromLocal8Bit("Â¥¶°ºÅ:"), buildingEdit);
    formLayout->addRow(QString::fromLocal8Bit("Â¥²ã:"), floorEdit);
    formLayout->addRow(QString::fromLocal8Bit("ËÞÉáºÅ:"), dormNumberEdit);
    formLayout->addRow(QString::fromLocal8Bit("·¿¼äÀàÐÍ:"), typeEdit);
    formLayout->addRow(QString::fromLocal8Bit("´²Î»ºÅ:"), bedIdEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept, Qt::UniqueConnection);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject, Qt::UniqueConnection);
}

QString AddDormitoryDialog::getBuilding() const { return buildingEdit->text(); }
QString AddDormitoryDialog::getFloor() const { return floorEdit->text(); }
QString AddDormitoryDialog::getDormNumber() const { return dormNumberEdit->text(); }
QString AddDormitoryDialog::getType() const { return typeEdit->text(); }
QString AddDormitoryDialog::getBedId() const { return bedIdEdit->text(); }

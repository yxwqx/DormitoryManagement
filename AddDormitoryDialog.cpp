#include "AddDormitoryDialog.h"

AddDormitoryDialog::AddDormitoryDialog(QWidget *parent, int nextDormNumber) : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("�������"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    buildingEdit = new QLineEdit(this);
    floorEdit = new QLineEdit(this);
    dormNumberEdit = new QLineEdit(this);
    typeEdit = new QLineEdit(this);
    bedIdEdit = new QLineEdit(this);

    formLayout->addRow(QString::fromLocal8Bit("¥����:"), buildingEdit);
    formLayout->addRow(QString::fromLocal8Bit("¥��:"), floorEdit);
    formLayout->addRow(QString::fromLocal8Bit("�����:"), dormNumberEdit);
    formLayout->addRow(QString::fromLocal8Bit("��������:"), typeEdit);
    formLayout->addRow(QString::fromLocal8Bit("��λ��:"), bedIdEdit);

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

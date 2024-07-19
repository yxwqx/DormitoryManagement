#include "AddStudentDialog.h"

AddStudentDialog::AddStudentDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("���ѧ��"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    idEdit = new QLineEdit(this);
    nameEdit = new QLineEdit(this);
    genderEdit = new QLineEdit(this);
    birthdateEdit = new QLineEdit(this);
    gradeEdit = new QLineEdit(this);
    majorEdit = new QLineEdit(this);
    classInEdit = new QLineEdit(this);
    phoneEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);

    formLayout->addRow(QString::fromLocal8Bit("ѧ��:"), idEdit);
    formLayout->addRow(QString::fromLocal8Bit("����:"), nameEdit);
    formLayout->addRow(QString::fromLocal8Bit("�Ա�:"), genderEdit);
    formLayout->addRow(QString::fromLocal8Bit("��������:"), birthdateEdit);
    formLayout->addRow(QString::fromLocal8Bit("�꼶:"), gradeEdit);
    formLayout->addRow(QString::fromLocal8Bit("רҵ:"), majorEdit);
    formLayout->addRow(QString::fromLocal8Bit("�༶:"), classInEdit);
    formLayout->addRow(QString::fromLocal8Bit("�绰:"), phoneEdit);
    formLayout->addRow(QString::fromLocal8Bit("����:"), passwordEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept, Qt::UniqueConnection);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject, Qt::UniqueConnection);
}

QString AddStudentDialog::getId() const { return idEdit->text(); }
QString AddStudentDialog::getName() const { return nameEdit->text(); }
QString AddStudentDialog::getGender() const { return genderEdit->text(); }
QString AddStudentDialog::getBirthdate() const { return birthdateEdit->text(); }
QString AddStudentDialog::getGrade() const { return gradeEdit->text(); }
QString AddStudentDialog::getMajor() const { return majorEdit->text(); }
QString AddStudentDialog::getClassIn() const { return classInEdit->text(); }
QString AddStudentDialog::getPhone() const { return phoneEdit->text(); }
QString AddStudentDialog::getPassword() const { return passwordEdit->text(); }

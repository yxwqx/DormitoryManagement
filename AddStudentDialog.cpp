#include "AddStudentDialog.h"

AddStudentDialog::AddStudentDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("添加学生"));
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

    formLayout->addRow(QString::fromLocal8Bit("学号:"), idEdit);
    formLayout->addRow(QString::fromLocal8Bit("姓名:"), nameEdit);
    formLayout->addRow(QString::fromLocal8Bit("性别:"), genderEdit);
    formLayout->addRow(QString::fromLocal8Bit("出生日期:"), birthdateEdit);
    formLayout->addRow(QString::fromLocal8Bit("年级:"), gradeEdit);
    formLayout->addRow(QString::fromLocal8Bit("专业:"), majorEdit);
    formLayout->addRow(QString::fromLocal8Bit("班级:"), classInEdit);
    formLayout->addRow(QString::fromLocal8Bit("电话:"), phoneEdit);
    formLayout->addRow(QString::fromLocal8Bit("密码:"), passwordEdit);

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

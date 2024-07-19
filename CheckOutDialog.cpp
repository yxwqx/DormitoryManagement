#include "CheckOutDialog.h"
#include "StudentManager.h"
#include "DormitoryManager.h"
#include <QMessageBox>

CheckOutDialog::CheckOutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("��������"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    studentIdEdit = new QLineEdit(this);
    studentNameLabel = new QLabel(this);
    buildingLabel = new QLabel(this);
    floorLabel = new QLabel(this);
    typeLabel = new QLabel(this);
    dormNumberLabel = new QLabel(this);
    bedIdLabel = new QLabel(this);

    formLayout->addRow(QString::fromLocal8Bit("ѧ��:"), studentIdEdit);
    formLayout->addRow(QString::fromLocal8Bit("����:"), studentNameLabel);
    formLayout->addRow(QString::fromLocal8Bit("¥��:"), buildingLabel);
    formLayout->addRow(QString::fromLocal8Bit("¥��:"), floorLabel);
    formLayout->addRow(QString::fromLocal8Bit("��������:"), typeLabel);
    formLayout->addRow(QString::fromLocal8Bit("�����:"), dormNumberLabel);
    formLayout->addRow(QString::fromLocal8Bit("��λ��:"), bedIdLabel);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(studentIdEdit, &QLineEdit::textChanged, this, &CheckOutDialog::onStudentIdChanged);
}

QString CheckOutDialog::getStudentId() const
{
    return studentIdEdit->text();
}

void CheckOutDialog::onStudentIdChanged() {
    QString studentId = studentIdEdit->text();
    StudentManager *studentManager = StudentManager::getinstance();
    vector<student> students = studentManager->get_student("id = '" + studentId.toStdString() + "'");

    if (students.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�ҵ���ѧ�ŵ�ѧ��"));
        return;
    }

    student &stu = students[0];
    studentNameLabel->setText(QString::fromLocal8Bit(stu.name.c_str()));

    DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
    vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id = '" + studentId.toStdString() + "'");

    if (dormitories.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��ѧ��δ��ס"));
        return;
    }

    Dormitory &dorm = dormitories[0];
    buildingLabel->setText(QString::fromLocal8Bit(dorm.building.c_str()));
    floorLabel->setText(QString::fromLocal8Bit(dorm.floor.c_str()));
    typeLabel->setText(QString::fromLocal8Bit(dorm.type.c_str()));
    dormNumberLabel->setText(QString::fromLocal8Bit(dorm.dorm_number.c_str()));
    bedIdLabel->setText(QString::fromLocal8Bit(dorm.bed_id.c_str()));
}

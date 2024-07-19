#include "CheckOutDialog.h"
#include "StudentManager.h"
#include "DormitoryManager.h"
#include <QMessageBox>

CheckOutDialog::CheckOutDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString::fromLocal8Bit("办理退宿"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    studentIdEdit = new QLineEdit(this);
    studentNameLabel = new QLabel(this);
    buildingLabel = new QLabel(this);
    floorLabel = new QLabel(this);
    typeLabel = new QLabel(this);
    dormNumberLabel = new QLabel(this);
    bedIdLabel = new QLabel(this);

    formLayout->addRow(QString::fromLocal8Bit("学号:"), studentIdEdit);
    formLayout->addRow(QString::fromLocal8Bit("姓名:"), studentNameLabel);
    formLayout->addRow(QString::fromLocal8Bit("楼号:"), buildingLabel);
    formLayout->addRow(QString::fromLocal8Bit("楼层:"), floorLabel);
    formLayout->addRow(QString::fromLocal8Bit("房间类型:"), typeLabel);
    formLayout->addRow(QString::fromLocal8Bit("宿舍号:"), dormNumberLabel);
    formLayout->addRow(QString::fromLocal8Bit("床位号:"), bedIdLabel);

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
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未找到该学号的学生"));
        return;
    }

    student &stu = students[0];
    studentNameLabel->setText(QString::fromLocal8Bit(stu.name.c_str()));

    DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
    vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id = '" + studentId.toStdString() + "'");

    if (dormitories.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("该学生未入住"));
        return;
    }

    Dormitory &dorm = dormitories[0];
    buildingLabel->setText(QString::fromLocal8Bit(dorm.building.c_str()));
    floorLabel->setText(QString::fromLocal8Bit(dorm.floor.c_str()));
    typeLabel->setText(QString::fromLocal8Bit(dorm.type.c_str()));
    dormNumberLabel->setText(QString::fromLocal8Bit(dorm.dorm_number.c_str()));
    bedIdLabel->setText(QString::fromLocal8Bit(dorm.bed_id.c_str()));
}

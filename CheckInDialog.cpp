#include "CheckInDialog.h"
#include "StudentManager.h"
#include "DormitoryManager.h"
#include "PaymentDialog.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>

CheckInDialog::CheckInDialog(MYSQL *dbConnection, QWidget *parent) : QDialog(parent), dbConnection(dbConnection)
{
    setWindowTitle(QString::fromLocal8Bit("办理入住"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    studentIdEdit = new QLineEdit(this);
    studentNameEdit = new QLineEdit(this);
    buildingEdit = new QLineEdit(this);
    floorComboBox = new QComboBox(this);
    typeComboBox = new QComboBox(this);
    dormNumberComboBox = new QComboBox(this);
    bedIdComboBox = new QComboBox(this);

    formLayout->addRow(QString::fromLocal8Bit("学号:"), studentIdEdit);
    formLayout->addRow(QString::fromLocal8Bit("姓名:"), studentNameEdit);
    formLayout->addRow(QString::fromLocal8Bit("楼号:"), buildingEdit);
    formLayout->addRow(QString::fromLocal8Bit("楼层:"), floorComboBox);
    formLayout->addRow(QString::fromLocal8Bit("房间类型:"), typeComboBox);
    formLayout->addRow(QString::fromLocal8Bit("宿舍号:"), dormNumberComboBox);
    formLayout->addRow(QString::fromLocal8Bit("床位号:"), bedIdComboBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &CheckInDialog::on_okButton_clicked);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    connect(studentIdEdit, &QLineEdit::textChanged, this, &CheckInDialog::onStudentIdChanged);
    connect(studentIdEdit, &QLineEdit::editingFinished, this, &CheckInDialog::onStudentIdEditingFinished);
}

CheckInDialog::~CheckInDialog()
{
}

void CheckInDialog::onStudentIdChanged()
{
    QString studentId = studentIdEdit->text();
    StudentManager *studentManager = StudentManager::getinstance();
    vector<student> students = studentManager->get_student("id = '" + studentId.toStdString() + "'");

    if (students.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("未找到该学号的学生"));
        return;
    }

    student &stu = students[0];
    studentNameEdit->setText(QString::fromLocal8Bit(stu.name.c_str()));
    buildingEdit->setText(stu.gender == "男" ? QString::fromLocal8Bit("1号") : QString::fromLocal8Bit("2号"));

    updateDormitoryInfo();
}

void CheckInDialog::onStudentIdEditingFinished()
{

}

void CheckInDialog::updateDormitoryInfo()
{
    DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
    vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id IS NULL");

    floorComboBox->clear();
    typeComboBox->clear();
    dormNumberComboBox->clear();
    bedIdComboBox->clear();

    for (const Dormitory &dorm : dormitories) {
        floorComboBox->addItem(QString::fromLocal8Bit(dorm.floor.c_str()));
        typeComboBox->addItem(QString::fromLocal8Bit(dorm.type.c_str()));
        dormNumberComboBox->addItem(QString::fromLocal8Bit(dorm.dorm_number.c_str()));
        bedIdComboBox->addItem(QString::fromLocal8Bit(dorm.bed_id.c_str()));
    }
}

void CheckInDialog::on_okButton_clicked()
{
    QString studentId = studentIdEdit->text();
    QString studentName = studentNameEdit->text();
    QString building = buildingEdit->text();
    QString floor = floorComboBox->currentText();
    QString roomType = typeComboBox->currentText();
    QString dormNumber = dormNumberComboBox->currentText();
    QString bedId = bedIdComboBox->currentText();

    QByteArray local8bitRoomType = roomType.toLocal8Bit();

    int fee = 0;
    if (local8bitRoomType == "单人间") {
        fee = 3000;
    } else if (local8bitRoomType == "双人间") {
        fee = 2500;
    } else if (local8bitRoomType == "四人间") {
        fee = 2000;
    } else if (local8bitRoomType == "六人间") {
        fee = 1500;
    }

    PaymentDialog paymentDialog(roomType, fee, this);
    if (paymentDialog.exec() == QDialog::Accepted) {
        QString paymentMethod = paymentDialog.getPaymentMethod();
        QString selectedSemester = paymentDialog.getSelectedSemester();

        QByteArray local8bitSemester = selectedSemester.toLocal8Bit();
        int semesterIndex = -1;
        if (local8bitSemester == "第一学期") {
            semesterIndex = 1;
        } else if (local8bitSemester == "第二学期") {
            semesterIndex = 2;
        } else if (local8bitSemester == "第三学期") {
            semesterIndex = 3;
        } else if (local8bitSemester == "第四学期") {
            semesterIndex = 4;
        } else if (local8bitSemester == "第五学期") {
            semesterIndex = 5;
        } else if (local8bitSemester == "第六学期") {
            semesterIndex = 6;
        } else if (local8bitSemester == "第七学期") {
            semesterIndex = 7;
        } else if (local8bitSemester == "第八学期") {
            semesterIndex = 8;
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("无效的学期选择"));
            return;
        }

        QString feeField = QString("fee%1").arg(semesterIndex);

        QString updateQuery = QString("UPDATE student SET %1 = %2, payment_method = '%3', check_in_date = NOW() WHERE id = '%4'")
                                  .arg(feeField)
                                  .arg(fee)
                                  .arg(paymentMethod)
                                  .arg(studentId);

        if (mysql_query(dbConnection, updateQuery.toLocal8Bit().constData())) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("更新学生缴费信息失败: %1").arg(QString::fromLocal8Bit(mysql_error(dbConnection))));
            return;
        }

        QString updateDormQuery = QString("UPDATE dormitory SET student_id = '%1' WHERE building = '%2' AND floor = '%3' AND dorm_number = '%4' AND bed_id = '%5'")
                                      .arg(studentId)
                                      .arg(building)
                                      .arg(floor)
                                      .arg(dormNumber)
                                      .arg(bedId);

        if (mysql_query(dbConnection, updateDormQuery.toLocal8Bit().constData())) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("更新宿舍信息失败: %1").arg(QString::fromLocal8Bit(mysql_error(dbConnection))));
            return;
        }

        accept();
    } else {

    }
}

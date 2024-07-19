#include "CheckInDialog.h"
#include "StudentManager.h"
#include "DormitoryManager.h"
#include "PaymentDialog.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QLabel>

CheckInDialog::CheckInDialog(MYSQL *dbConnection, QWidget *parent) : QDialog(parent), dbConnection(dbConnection)
{
    setWindowTitle(QString::fromLocal8Bit("������ס"));
    setModal(true);

    QFormLayout *formLayout = new QFormLayout(this);

    studentIdEdit = new QLineEdit(this);
    studentNameEdit = new QLineEdit(this);
    buildingEdit = new QLineEdit(this);
    floorComboBox = new QComboBox(this);
    typeComboBox = new QComboBox(this);
    dormNumberComboBox = new QComboBox(this);
    bedIdComboBox = new QComboBox(this);

    formLayout->addRow(QString::fromLocal8Bit("ѧ��:"), studentIdEdit);
    formLayout->addRow(QString::fromLocal8Bit("����:"), studentNameEdit);
    formLayout->addRow(QString::fromLocal8Bit("¥��:"), buildingEdit);
    formLayout->addRow(QString::fromLocal8Bit("¥��:"), floorComboBox);
    formLayout->addRow(QString::fromLocal8Bit("��������:"), typeComboBox);
    formLayout->addRow(QString::fromLocal8Bit("�����:"), dormNumberComboBox);
    formLayout->addRow(QString::fromLocal8Bit("��λ��:"), bedIdComboBox);

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
        QMessageBox::warning(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("δ�ҵ���ѧ�ŵ�ѧ��"));
        return;
    }

    student &stu = students[0];
    studentNameEdit->setText(QString::fromLocal8Bit(stu.name.c_str()));
    buildingEdit->setText(stu.gender == "��" ? QString::fromLocal8Bit("1��") : QString::fromLocal8Bit("2��"));

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
    if (local8bitRoomType == "���˼�") {
        fee = 3000;
    } else if (local8bitRoomType == "˫�˼�") {
        fee = 2500;
    } else if (local8bitRoomType == "���˼�") {
        fee = 2000;
    } else if (local8bitRoomType == "���˼�") {
        fee = 1500;
    }

    PaymentDialog paymentDialog(roomType, fee, this);
    if (paymentDialog.exec() == QDialog::Accepted) {
        QString paymentMethod = paymentDialog.getPaymentMethod();
        QString selectedSemester = paymentDialog.getSelectedSemester();

        QByteArray local8bitSemester = selectedSemester.toLocal8Bit();
        int semesterIndex = -1;
        if (local8bitSemester == "��һѧ��") {
            semesterIndex = 1;
        } else if (local8bitSemester == "�ڶ�ѧ��") {
            semesterIndex = 2;
        } else if (local8bitSemester == "����ѧ��") {
            semesterIndex = 3;
        } else if (local8bitSemester == "����ѧ��") {
            semesterIndex = 4;
        } else if (local8bitSemester == "����ѧ��") {
            semesterIndex = 5;
        } else if (local8bitSemester == "����ѧ��") {
            semesterIndex = 6;
        } else if (local8bitSemester == "����ѧ��") {
            semesterIndex = 7;
        } else if (local8bitSemester == "�ڰ�ѧ��") {
            semesterIndex = 8;
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("��Ч��ѧ��ѡ��"));
            return;
        }

        QString feeField = QString("fee%1").arg(semesterIndex);

        QString updateQuery = QString("UPDATE student SET %1 = %2, payment_method = '%3', check_in_date = NOW() WHERE id = '%4'")
                                  .arg(feeField)
                                  .arg(fee)
                                  .arg(paymentMethod)
                                  .arg(studentId);

        if (mysql_query(dbConnection, updateQuery.toLocal8Bit().constData())) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����ѧ���ɷ���Ϣʧ��: %1").arg(QString::fromLocal8Bit(mysql_error(dbConnection))));
            return;
        }

        QString updateDormQuery = QString("UPDATE dormitory SET student_id = '%1' WHERE building = '%2' AND floor = '%3' AND dorm_number = '%4' AND bed_id = '%5'")
                                      .arg(studentId)
                                      .arg(building)
                                      .arg(floor)
                                      .arg(dormNumber)
                                      .arg(bedId);

        if (mysql_query(dbConnection, updateDormQuery.toLocal8Bit().constData())) {
            QMessageBox::critical(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("����������Ϣʧ��: %1").arg(QString::fromLocal8Bit(mysql_error(dbConnection))));
            return;
        }

        accept();
    } else {

    }
}

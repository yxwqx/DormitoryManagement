#include "PaymentDialog.h"
#include "ui_PaymentDialog.h"

PaymentDialog::PaymentDialog(QString roomType, int fee, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PaymentDialog)
{
    ui->setupUi(this);
    QString message = QString("���ķ���Ϊ %1 ��ɷ� %2 Ԫ").arg(roomType).arg(fee);
    ui->label->setText(message);
}

PaymentDialog::~PaymentDialog()
{
    delete ui;
}

QString PaymentDialog::getPaymentMethod() const
{
    return paymentMethod;
}

QString PaymentDialog::getSelectedSemester() const
{
    return selectedSemester;
}

int PaymentDialog::getSelectedSemesterIndex() const // ʵ�ֺ���
{
    return ui->semesterComboBox->currentIndex();
}

void PaymentDialog::on_okButton_clicked()
{
    paymentMethod = ui->paymentMethodComboBox->currentText();
    selectedSemester = ui->semesterComboBox->currentText();
    accept();
}

void PaymentDialog::on_cancelButton_clicked()
{
    reject();
}

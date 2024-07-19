#ifndef PAYMENTDIALOG_H
#define PAYMENTDIALOG_H

#include <QDialog>

namespace Ui {
class PaymentDialog;
}

class PaymentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PaymentDialog(QString roomType, int fee, QWidget *parent = nullptr);
    ~PaymentDialog();

    QString getPaymentMethod() const;
    QString getSelectedSemester() const;
    int getSelectedSemesterIndex() const; // 新增函数

private slots:
    void on_okButton_clicked();
    void on_cancelButton_clicked();

private:
    Ui::PaymentDialog *ui;
    QString paymentMethod;
    QString selectedSemester;
};

#endif // PAYMENTDIALOG_H

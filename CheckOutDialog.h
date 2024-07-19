#ifndef CHECKOUTDIALOG_H
#define CHECKOUTDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLabel>

class CheckOutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckOutDialog(QWidget *parent = nullptr);

    QString getStudentId() const;

private slots:
    void onStudentIdChanged();

private:
    QLineEdit *studentIdEdit;
    QLabel *studentNameLabel;
    QLabel *buildingLabel;
    QLabel *floorLabel;
    QLabel *typeLabel;
    QLabel *dormNumberLabel;
    QLabel *bedIdLabel;
};

#endif // CHECKOUTDIALOG_H

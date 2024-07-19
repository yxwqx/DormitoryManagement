#ifndef CHECKINDIALOG_H
#define CHECKINDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <mysql.h>

class CheckInDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CheckInDialog(MYSQL *dbConnection, QWidget *parent = nullptr);
    ~CheckInDialog();
    QLineEdit *studentIdEdit;
    QLineEdit *studentNameEdit;
    QLineEdit *buildingEdit;
    QComboBox *floorComboBox;
    QComboBox *typeComboBox;
    QComboBox *dormNumberComboBox;
    QComboBox *bedIdComboBox;
    QDialogButtonBox *buttonBox;

private slots:
    void onStudentIdChanged();
    void onStudentIdEditingFinished();
    void on_okButton_clicked();

private:
    void updateDormitoryInfo();
    void checkStudentExists();
    MYSQL *dbConnection;
};

#endif // CHECKINDIALOG_H

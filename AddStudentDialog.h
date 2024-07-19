#ifndef ADDSTUDENTDIALOG_H
#define ADDSTUDENTDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

class AddStudentDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddStudentDialog(QWidget *parent = nullptr);

    QString getId() const;
    QString getName() const;
    QString getGender() const;
    QString getBirthdate() const;
    QString getGrade() const;
    QString getMajor() const;
    QString getClassIn() const;
    QString getPhone() const;
    QString getPassword() const;

private:
    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QLineEdit *genderEdit;
    QLineEdit *birthdateEdit;
    QLineEdit *gradeEdit;
    QLineEdit *majorEdit;
    QLineEdit *classInEdit;
    QLineEdit *phoneEdit;
    QLineEdit *passwordEdit;
};

#endif // ADDSTUDENTDIALOG_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
#include <QStandardItem>
#include "StudentManager.h"
#include "AddStudentDialog.h"
#include "DormitoryManager.h"
#include "AnnouncementDialog.h"
#include <mysql.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateDormitoryTable();
    void on_repairRequestButton_clicked();

protected:
    void resizeEvent(QResizeEvent* event);

private slots:
    void on_loginButton_clicked();
    void on_forgotPasswordButton_clicked();
    void on_editButton_clicked(const QModelIndex &index);
    void on_deleteButton_clicked(const QModelIndex &index);
    void updateStudentTable();
    void updateCheckInTable();
    void on_adminLoginButton_clicked();
    void on_studentLoginButton_clicked();
    void onAdminMenuItemClicked(QTreeWidgetItem *item, int column);
    void on_searchButton_clicked();
    void on_addStudentButton_clicked();
    void on_searchDormitoryButton_clicked();
    void on_addDormitoryButton_clicked();
    void on_editDormitoryButton_clicked(const QModelIndex &index);
    void on_deleteDormitoryButton_clicked(const QModelIndex &index);
    void on_logoutButton_clicked();
    void on_checkInButton_clicked();
    void onStudentIdEditingFinished();
    void on_checkOutButton_clicked();
    void updateCheckOutTable();
    void toggleMenuItem(QTreeWidgetItem *item, int column);
    void on_announcementSendButton_clicked();
    void updateAnnouncementList();
    void on_searchRepairButton_clicked();
    void on_viewRepairButton_clicked();
    void on_replyRepairButton_clicked();
    void on_viewReplyRepairButton_clicked(const QModelIndex &index);
    void on_deleteRepairButton_clicked(const QModelIndex &index);
    void on_viewAnnouncementButton_clicked();

private:
    Ui::MainWindow *ui;
    StudentManager *manager;
    DormitoryManager *manager2;
    MYSQL *con;
    bool isAdminLogin;
    void showWelcomeMenu();
    void showLoginDialog();
    void showAdminMenu();
    void showStudentMenu();
    void setupAdminMenu();
    void setupTreeWidget();
    void updateRepairTable();
    void displayStudentInfo(const vector<student> &students);
    void displayDormitoryInfo(const vector<Dormitory> &dormitories);
    QTreeWidgetItem* lastClickedItem;
    bool isMenuVisible;
    QString currentStudentId;
    AnnouncementDialog *announcementDialog;
    bool isAnnouncementDialogVisible;
};

#endif // MAINWINDOW_H

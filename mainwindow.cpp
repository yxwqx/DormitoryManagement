#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QDialog>
#include <QFormLayout>
#include <QSqlQuery>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDebug>
#include <QDateTime>
#include "AddDormitoryDialog.h"
#include "AddStudentDialog.h"
#include "CheckInDialog.h"
#include "CheckOutDialog.h"
#include "ButtonDelegate.h"
#include "RepairButtonDelegate.h"
#include "AnnouncementDialog.h"

#define UNUSED(x) (void)(x)

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , isAdminLogin(false)
    , lastClickedItem(nullptr)
    , isMenuVisible(true)
    , isAnnouncementDialogVisible(false)
{
    ui->setupUi(this);
    manager = StudentManager::getinstance();
    manager2 = DormitoryManager::getInstance();
    announcementDialog = new AnnouncementDialog(this);
    con = mysql_init(NULL);
    if (con == NULL) {
        cerr << "mysql_init() failed" << endl;
        exit(1);
    }
    mysql_options(con, MYSQL_SET_CHARSET_NAME, "gbk");
    if (!mysql_real_connect(con, "127.0.0.1", "root", "0199wbBB", "student_data", 3306, NULL, 0)) {
        cerr << "连接数据库失败: " << mysql_error(con) << endl;
        mysql_close(con);
        exit(1);
    }
    if (mysql_set_character_set(con, "gbk")) {
        cerr << "设置字符集失败: " << mysql_error(con) << endl;
    }

    setFixedSize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);

    QPixmap background("C:/Users/yxwqx/Documents/coding3/loginbackground.jpg");
    QPalette palette;
    palette.setBrush(QPalette::Window, background.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setPalette(palette);
    ui->loginButton->setStyleSheet("padding: 10px; background-color: #4CAF50; color: white; border: none; border-radius: 5px;");
    ui->forgotPasswordButton->setStyleSheet("padding: 10px; background-color: #f44336; color: white; border: none; border-radius: 5px;");
    ui->usernameLineEdit->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;");
    ui->passwordLineEdit->setStyleSheet("padding: 10px; border: 1px solid #ccc; border-radius: 5px;");
    ui->adminRadioButton->setStyleSheet("font-size: 12px;");
    ui->studentRadioButton->setStyleSheet("font-size: 12px;");
    ui->stackedWidget->addWidget(ui->welcomePage);
    ui->stackedWidget->addWidget(ui->adminPage);
    ui->stackedWidget->addWidget(ui->studentPage);
    ui->stackedWidget->addWidget(ui->announcementPage);

    ui->adminStackedWidget->addWidget(ui->adminWelcomePage);
    ui->adminStackedWidget->addWidget(ui->studentManagementPage);
    ui->adminStackedWidget->addWidget(ui->dormitoryManagementPage);
    ui->adminStackedWidget->addWidget(ui->checkInPage);
    ui->adminStackedWidget->addWidget(ui->checkOutPage);
    ui->adminStackedWidget->addWidget(ui->announcementPage);
    ui->adminStackedWidget->addWidget(ui->repairManagementPage);
    ui->adminMenuTreeWidget->setFixedWidth(200);
    RepairButtonDelegate *repairDelegate = new RepairButtonDelegate(this);
    connect(repairDelegate, &RepairButtonDelegate::viewReplyButtonClicked, this, &MainWindow::on_viewReplyRepairButton_clicked);
    connect(repairDelegate, &RepairButtonDelegate::deleteButtonClicked, this, &MainWindow::on_deleteRepairButton_clicked);
    ui->repairTableView->setItemDelegateForColumn(4, repairDelegate);
    connect(ui->adminMenuTreeWidget, &QTreeWidget::itemClicked, this, &MainWindow::toggleMenuItem);
    connect(ui->loginButton, &QPushButton::clicked, this, &MainWindow::on_loginButton_clicked);
    connect(ui->forgotPasswordButton, &QPushButton::clicked, this, &MainWindow::on_forgotPasswordButton_clicked);
    connect(ui->adminRadioButton, &QPushButton::clicked, this, &MainWindow::on_adminLoginButton_clicked);
    connect(ui->adminMenuTreeWidget, &QTreeWidget::itemClicked, this, &MainWindow::toggleMenuItem);
    connect(ui->adminMenuTreeWidget, &QTreeWidget::itemClicked, this, &MainWindow::onAdminMenuItemClicked);
    connect(ui->searchButton, &QPushButton::clicked, this, &MainWindow::on_searchButton_clicked);
    connect(ui->addStudentButton, &QPushButton::clicked, this, &MainWindow::on_addStudentButton_clicked, Qt::UniqueConnection);
    connect(ui->adminMenuTreeWidget, &QTreeWidget::itemClicked, this, &MainWindow::onAdminMenuItemClicked);
    connect(ui->logoutButton, &QPushButton::clicked, this, &MainWindow::on_logoutButton_clicked);
    connect(ui->viewAnnouncementButton, &QPushButton::clicked, this, &MainWindow::on_viewAnnouncementButton_clicked);
    connect(ui->repairRequestButton, &QPushButton::clicked, this, &MainWindow::on_repairRequestButton_clicked);
    ButtonDelegate *studentDelegate = new ButtonDelegate(this);
    connect(studentDelegate, &ButtonDelegate::editButtonClicked, this, &MainWindow::on_editButton_clicked);
    connect(studentDelegate, &ButtonDelegate::deleteButtonClicked, this, &MainWindow::on_deleteButton_clicked);
    ui->studentTableView->setItemDelegateForColumn(10, studentDelegate);
    ButtonDelegate *dormDelegate = new ButtonDelegate(this);
    connect(dormDelegate, &ButtonDelegate::editButtonClicked, this, &MainWindow::on_editDormitoryButton_clicked);
    connect(dormDelegate, &ButtonDelegate::deleteButtonClicked, this, &MainWindow::on_deleteDormitoryButton_clicked);
    ui->dormitoryTableView->setItemDelegateForColumn(7, dormDelegate);
    updateRepairTable();
    setupAdminMenu();
    showLoginDialog();
}

MainWindow::~MainWindow()
{
    if (con) {
        mysql_close(con);
    }
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    QPixmap background("C:/Users/yxwqx/Documents/coding3/loginbackground.jpg");
    QPalette palette;
    palette.setBrush(QPalette::Window, background.scaled(this->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    this->setPalette(palette);
}
void onStudentIdEditingFinished();
void MainWindow::on_deleteRepairButton_clicked(const QModelIndex &index)
{
    QString studentId = ui->repairTableView->model()->index(index.row(), 0).data().toString();
    QString content = ui->repairTableView->model()->index(index.row(), 1).data().toString();
    qDebug() << "Delete repair clicked: " << studentId << content;

    if (QMessageBox::question(this, "删除报修", "确定要删除这条报修信息吗？", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        QString queryStr = QString("DELETE FROM repair_order WHERE student_re_id = '%1' AND content = '%2'")
                               .arg(studentId)
                               .arg(content);

        if (mysql_query(con, queryStr.toLocal8Bit().constData())) {
            QMessageBox::critical(this, "错误", "删除报修失败: " + QString::fromLocal8Bit(mysql_error(con)));
        } else {
            QMessageBox::information(this, "成功", "删除报修成功！");
            updateRepairTable();
        }
    }
}

void MainWindow::toggleMenuItem(QTreeWidgetItem *item, int column)
{
    UNUSED(column);
    if (ui->adminStackedWidget->isVisible()) {
        ui->adminStackedWidget->hide();
        ui->adminStackedWidget->setCurrentWidget(ui->adminWelcomePage);
    } else {
        ui->adminStackedWidget->show();
    }

    if (lastClickedItem == item) {
        return;
    }

    int index = item->data(0, Qt::UserRole).toInt();
    qDebug() << "Menu item clicked:" << item->text(0) << "with index:" << index;

    switch (index) {
    case 1:
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->studentManagementPage));
        updateStudentTable();
        break;
    case 2:
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->dormitoryManagementPage));
        updateDormitoryTable();
        break;
    case 3:
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->checkInPage));
        updateCheckInTable();
        break;
    case 4:
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->checkOutPage));
        updateCheckOutTable();
        break;
    default:
        qDebug() << "Unknown index";
        break;
    }

    lastClickedItem = item;
}

void MainWindow::on_loginButton_clicked()
{
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (isAdminLogin) {
        if (manager->validate_admin_login(username.toStdString(), password.toStdString())) {
            showAdminMenu();
        } else {
            QMessageBox::warning(this, QString::fromLocal8Bit("登录失败"), QString::fromLocal8Bit("管理员用户名或密码错误"));
        }
    } else {
        if (manager->validate_student_login(username.toStdString(), password.toStdString())) {
            showStudentMenu();
            displayStudentInfo(manager->get_student("id = '" + username.toStdString() + "'"));
            displayDormitoryInfo(manager2->get_dormitory("student_id = '" + username.toStdString() + "'"));
            ui->stackedWidget->setCurrentWidget(ui->studentPage);
        } else {
            QMessageBox::warning(this, QString::fromLocal8Bit("登录失败"), QString::fromLocal8Bit("学生用户名或密码错误"));
        }
    }
}

void MainWindow::on_logoutButton_clicked() {
    isAdminLogin = false;
    showLoginDialog();
    setMinimumSize(800, 600);
    setMaximumSize(800, 600);
    setFixedSize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();
    ui->adminRadioButton->setAutoExclusive(false);
    ui->adminRadioButton->setChecked(false);
    ui->adminRadioButton->setAutoExclusive(true);
    ui->studentRadioButton->setAutoExclusive(false);
    ui->studentRadioButton->setChecked(false);
    ui->studentRadioButton->setAutoExclusive(true);
}

void MainWindow::on_forgotPasswordButton_clicked()
{
    QMessageBox::information(this, QString::fromLocal8Bit("忘记密码"), QString::fromLocal8Bit("请联系管理员重置密码。"));
}

void MainWindow::on_adminLoginButton_clicked()
{
    isAdminLogin = true;
}

void MainWindow::on_studentLoginButton_clicked()
{
    QString studentId = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();

    if (manager->validate_student_login(studentId.toStdString(), password.toStdString())) {
        currentStudentId = studentId;
        vector<student> students = manager->get_student("id = '" + studentId.toStdString() + "'");
        displayStudentInfo(students);
        vector<Dormitory> dormitories = manager2->get_dormitory("student_id = '" + studentId.toStdString() + "'");
        displayDormitoryInfo(dormitories);

        QMessageBox::information(this, tr("登录成功"), tr("学生登录成功"));
    } else {
        QMessageBox::warning(this, tr("登录失败"), tr("学号或密码错误"));
    }
}

void MainWindow::onAdminMenuItemClicked(QTreeWidgetItem *item, int column)
{
    UNUSED(column);
    int index = item->data(0, Qt::UserRole).toInt();
    qDebug() << "Menu item clicked:" << item->text(0) << "with index:" << index;

    switch (index) {
    case 1:
        qDebug() << "Switching to studentManagementPage";
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->studentManagementPage));
        updateStudentTable();
        break;
    case 2:
        qDebug() << "Switching to dormitoryManagementPage";
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->dormitoryManagementPage));
        updateDormitoryTable();
        break;
    case 3:
        qDebug() << "Switching to checkInPage";
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->checkInPage));
        updateCheckInTable();
        break;
    case 4:
        qDebug() << "Switching to checkOutPage";
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->checkOutPage));
        updateCheckOutTable();
        break;
    case 5:
        qDebug() << "Switching to announcementManagementPage";
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->announcementPage));
        updateAnnouncementList();
        break;
    case 6:
        qDebug() << "Switching to repairManagementPage";
        ui->adminStackedWidget->setCurrentIndex(ui->adminStackedWidget->indexOf(ui->repairManagementPage));
        updateRepairTable();
        break;
    default:
        qDebug() << "Unknown index";
        break;
    }

    lastClickedItem = item;
}

void MainWindow::on_announcementSendButton_clicked()
{
    QString announcementText = ui->announcementTextEdit->toPlainText();
    if (announcementText.isEmpty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("公告内容不能为空！"));
        return;
    }

    QString insertQuery = QString("INSERT INTO announcement (content) VALUES ('%1')")
                              .arg(announcementText);

    if (mysql_query(con, insertQuery.toLocal8Bit().constData())) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("发布公告失败:%1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    updateAnnouncementList();
    ui->announcementTextEdit->clear();
    QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("公告发布成功！"));
}

void MainWindow::updateAnnouncementList()
{
    ui->announcementListWidget->clear();

    QString selectQuery = "SELECT content, created_at FROM announcement ORDER BY created_at DESC";
    if (mysql_query(con, selectQuery.toLocal8Bit().constData())) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取公告列表失败: %1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == nullptr) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取公告结果失败: %1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        QString announcement = QString::fromLocal8Bit(row[0]);
        QString timestamp = QString::fromLocal8Bit(row[1]);
        QString displayText = QString("%1 [%2]").arg(announcement).arg(timestamp);
        ui->announcementListWidget->addItem(displayText);
    }

    mysql_free_result(result);
}

void MainWindow::updateRepairTable()
{
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList()
                                     << QString::fromLocal8Bit("学号")
                                     << QString::fromLocal8Bit("报修内容")
                                     << QString::fromLocal8Bit("回复")
                                     << QString::fromLocal8Bit("创建时间")
                                     << QString::fromLocal8Bit("操作"));

    if (mysql_query(con, "SELECT student_re_id, content, reply, created_at FROM repair_order")) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("查询报修表失败: %1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == nullptr) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取报修结果失败: %1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        QList<QStandardItem *> items;
        for(int i = 0; i < num_fields; i++) {
            items.append(new QStandardItem(QString::fromLocal8Bit(row[i])));
        }
        items.append(new QStandardItem(""));
        model->appendRow(items);
    }

    ui->repairTableView->setModel(model);
    ui->repairTableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::showAdminMenu()
{
    setMinimumSize(800, 600);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    showMaximized();
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    show();

    ui->stackedWidget->setCurrentIndex(1);
    ui->adminStackedWidget->setCurrentWidget(ui->adminWelcomePage);
    ui->logoutButton->show();
}

void MainWindow::showLoginDialog()
{
    setMinimumSize(800, 600);
    setMaximumSize(800, 600);
    setFixedSize(800, 600);
    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    show();
    ui->stackedWidget->setCurrentIndex(0);
    ui->logoutButton->hide();
}

void MainWindow::showStudentMenu()
{
    ui->stackedWidget->setCurrentWidget(ui->studentPage);
    ui->studentInfoTextEdit->clear();
    ui->dormitoryInfoTextEdit->clear();
    if (!currentStudentId.isEmpty()) {
        vector<student> students = manager->get_student("id = '" + currentStudentId.toStdString() + "'");
        displayStudentInfo(students);

        vector<Dormitory> dormitories = manager2->get_dormitory("student_id = '" + currentStudentId.toStdString() + "'");
        displayDormitoryInfo(dormitories);
    }
}

void MainWindow::setupAdminMenu()
{
    QTreeWidgetItem *basicManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    basicManagement->setText(0, QString::fromLocal8Bit("基本管理"));

    QTreeWidgetItem *studentManagement = new QTreeWidgetItem(basicManagement);
    studentManagement->setText(0, QString::fromLocal8Bit("学生管理"));
    studentManagement->setData(0, Qt::UserRole, 1);

    QTreeWidgetItem *dormitoryManagement = new QTreeWidgetItem(basicManagement);
    dormitoryManagement->setText(0, QString::fromLocal8Bit("宿舍管理"));
    dormitoryManagement->setData(0, Qt::UserRole, 2);

    QTreeWidgetItem *checkInManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    checkInManagement->setText(0, QString::fromLocal8Bit("入住、退宿管理"));

    QTreeWidgetItem *studentCheckIn = new QTreeWidgetItem(checkInManagement);
    studentCheckIn->setText(0, QString::fromLocal8Bit("学生入住"));
    studentCheckIn->setData(0, Qt::UserRole, 3);

    QTreeWidgetItem *studentCheckOut = new QTreeWidgetItem(checkInManagement);
    studentCheckOut->setText(0, QString::fromLocal8Bit("学生退宿"));
    studentCheckOut->setData(0, Qt::UserRole, 4);

    QTreeWidgetItem *announcementManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    announcementManagement->setText(0, QString::fromLocal8Bit("公告管理"));
    announcementManagement->setData(0, Qt::UserRole, 5);

    QTreeWidgetItem *repairManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    repairManagement->setText(0, QString::fromLocal8Bit("报修管理"));
    repairManagement->setData(0, Qt::UserRole, 6);

    ui->adminMenuTreeWidget->expandAll();
}

void MainWindow::setupTreeWidget()
{
    QTreeWidgetItem *basicManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    basicManagement->setText(0, QString::fromLocal8Bit("基本管理"));

    QTreeWidgetItem *studentManagement = new QTreeWidgetItem(basicManagement);
    studentManagement->setText(0, QString::fromLocal8Bit("学生管理"));
    studentManagement->setData(0, Qt::UserRole, 1);

    QTreeWidgetItem *dormitoryManagement = new QTreeWidgetItem(basicManagement);
    dormitoryManagement->setText(0, QString::fromLocal8Bit("宿舍管理"));
    dormitoryManagement->setData(0, Qt::UserRole, 2);

    QTreeWidgetItem *checkInManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    checkInManagement->setText(0, QString::fromLocal8Bit("入住、退宿管理"));

    QTreeWidgetItem *studentCheckIn = new QTreeWidgetItem(checkInManagement);
    studentCheckIn->setText(0, QString::fromLocal8Bit("学生入住"));
    studentCheckIn->setData(0, Qt::UserRole, 3);

    QTreeWidgetItem *studentCheckOut = new QTreeWidgetItem(checkInManagement);
    studentCheckOut->setText(0, QString::fromLocal8Bit("学生退宿"));
    studentCheckOut->setData(0, Qt::UserRole, 4);

    QTreeWidgetItem *otherManagement = new QTreeWidgetItem(ui->adminMenuTreeWidget);
    otherManagement->setText(0, QString::fromLocal8Bit("其他管理"));

    QTreeWidgetItem *addOther = new QTreeWidgetItem(otherManagement);
    addOther->setText(0, QString::fromLocal8Bit("添加其他"));
    addOther->setData(0, Qt::UserRole, 5);

    QTreeWidgetItem *queryOther = new QTreeWidgetItem(otherManagement);
    queryOther->setText(0, QString::fromLocal8Bit("查询其他"));
    queryOther->setData(0, Qt::UserRole, 6);

    ui->adminMenuTreeWidget->expandAll();
}

void MainWindow::on_searchButton_clicked()
{
    QString grade = QString::fromLocal8Bit(ui->gradeLineEdit->text().toLocal8Bit());
    QString major = QString::fromLocal8Bit(ui->majorLineEdit->text().toLocal8Bit());
    QString classin = QString::fromLocal8Bit(ui->classLineEdit->text().toLocal8Bit());
    QString name = QString::fromLocal8Bit(ui->nameLineEdit->text().toLocal8Bit());
    QString condition = "1=1";
    if (!grade.isEmpty()) {
        condition += QString(" AND grade LIKE '%%1%'").arg(grade);
    }
    if (!major.isEmpty()) {
        condition += QString(" AND major LIKE '%%1%'").arg(major);
    }
    if (!classin.isEmpty()) {
        condition += QString(" AND classin LIKE '%%1%'").arg(classin);
    }
    if (!name.isEmpty()) {
        condition += QString(" AND name LIKE '%%1%'").arg(name);
    }
    auto students = manager->get_student(condition.toLocal8Bit().constData());
    displayStudentInfo(students);
}

void MainWindow::on_searchDormitoryButton_clicked()
{
    QString building = QString::fromLocal8Bit(ui->buildingLineEdit->text().toLocal8Bit());
    QString floor = QString::fromLocal8Bit(ui->floorLineEdit->text().toLocal8Bit());
    QString dormNumber = QString::fromLocal8Bit(ui->dormNumberLineEdit->text().toLocal8Bit());
    QString type = QString::fromLocal8Bit(ui->typeLineEdit->text().toLocal8Bit());
    QString bedId = QString::fromLocal8Bit(ui->bedIdLineEdit->text().toLocal8Bit());
    QString condition = "1=1";
    if (!building.isEmpty()) {
        condition += QString(" AND building LIKE '%%1%'").arg(building);
    }
    if (!floor.isEmpty()) {
        condition += QString(" AND floor LIKE '%%1%'").arg(floor);
    }
    if (!dormNumber.isEmpty()) {
        condition += QString(" AND dorm_number LIKE '%%1%'").arg(dormNumber);
    }
    if (!type.isEmpty()) {
        condition += QString(" AND type LIKE '%%1%'").arg(type);
    }
    if (!bedId.isEmpty()) {
        condition += QString(" AND bed_id LIKE '%%1%'").arg(bedId);
    }
    qDebug() << "Generated condition:" << condition;

    auto dormitories = DormitoryManager::getInstance()->get_dormitory(condition.toLocal8Bit().constData());
    displayDormitoryInfo(dormitories);
}

void MainWindow::on_searchRepairButton_clicked()
{
    QString studentId = ui->searchStudentIdLineEdit->text();
    QString content = ui->searchContentLineEdit->text();
    QString queryStr = "SELECT student_re_id, content, reply, created_at FROM repair_order WHERE 1=1";

    if (!studentId.isEmpty()) {
        queryStr += " AND student_re_id LIKE '%" + studentId + "%'";
    }
    if (!content.isEmpty()) {
        queryStr += " AND content LIKE '%" + content + "%'";
    }

    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList()
                                     << QString::fromLocal8Bit("学号")
                                     << QString::fromLocal8Bit("报修内容")
                                     << QString::fromLocal8Bit("回复")
                                     << QString::fromLocal8Bit("创建时间"));
    ui->repairTableView->setModel(model);

    if (mysql_query(con, queryStr.toLocal8Bit().constData())) {
        QMessageBox::critical(this, "错误", "查询报修表失败: " + QString::fromLocal8Bit(mysql_error(con        )));
        return;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == nullptr) {
        QMessageBox::critical(this, "错误", "获取报修结果失败: " + QString::fromLocal8Bit(mysql_error(con)));
        return;
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result))) {
        QList<QStandardItem *> items;
        for(int i = 0; i < num_fields; i++) {
            items.append(new QStandardItem(QString::fromLocal8Bit(row[i])));
        }
        model->appendRow(items);
    }

    mysql_free_result(result);
}

void MainWindow::on_viewRepairButton_clicked()
{
    QModelIndex index = ui->repairTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("请选择一条报修信息"));
        return;
    }

    QString studentId = index.sibling(index.row(), 0).data().toString();
    QString content = index.sibling(index.row(), 1).data().toString();
    QString reply = index.sibling(index.row(), 2).data().toString();

    QDialog replyDialog(this);
    replyDialog.setWindowTitle(QString::fromLocal8Bit("查看/回复报修"));
    QVBoxLayout *layout = new QVBoxLayout(&replyDialog);

    QLabel *contentLabel = new QLabel(QString::fromLocal8Bit("报修内容:"), &replyDialog);
    QLabel *contentValue = new QLabel(content, &replyDialog);

    QLabel *replyLabel = new QLabel(QString::fromLocal8Bit("回复内容:"), &replyDialog);
    QLineEdit *replyEdit = new QLineEdit(reply, &replyDialog);

    QPushButton *okButton = new QPushButton(QString::fromLocal8Bit("回复"), &replyDialog);
    connect(okButton, &QPushButton::clicked, [&]() {
        QString newReply = replyEdit->text();
        QString queryStr = QString("UPDATE repair_order SET reply = '%1' WHERE student_re_id = '%2' AND content = '%3'")
                               .arg(newReply)
                               .arg(studentId)
                               .arg(content);

        if (mysql_query(con, queryStr.toLocal8Bit().constData())) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("回复报修失败: ") + QString::fromLocal8Bit(mysql_error(con)));
        } else {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("回复报修成功！"));
            updateRepairTable();
        }
        replyDialog.accept();
    });

    layout->addWidget(contentLabel);
    layout->addWidget(contentValue);
    layout->addWidget(replyLabel);
    layout->addWidget(replyEdit);
    layout->addWidget(okButton);

    replyDialog.exec();
}

void MainWindow::on_viewReplyRepairButton_clicked(const QModelIndex &index)
{
    QString studentId = ui->repairTableView->model()->index(index.row(), 0).data().toString();
    QString content = ui->repairTableView->model()->index(index.row(), 1).data().toString();
    qDebug() << "View/Reply repair clicked: " << studentId << content;
}

void MainWindow::on_repairRequestButton_clicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromLocal8Bit("故障报修"));
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLineEdit *studentIdEdit = new QLineEdit(currentStudentId, &dialog);
    QLineEdit *contentEdit = new QLineEdit(&dialog);
    contentEdit->setPlaceholderText(QString::fromLocal8Bit("请输入报修内容"));
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    layout->addWidget(new QLabel(QString::fromLocal8Bit("学号:"), &dialog));
    layout->addWidget(studentIdEdit);
    layout->addWidget(new QLabel(QString::fromLocal8Bit("报修内容:"), &dialog));
    layout->addWidget(contentEdit);
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() == QDialog::Accepted) {
        QString studentId = studentIdEdit->text();
        QString content = contentEdit->text();

        if (content.isEmpty()) {
            QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("报修内容不能为空！"));
            return;
        }
        QDateTime currentTime = QDateTime::currentDateTime();
        QString currentTimeStr = currentTime.toString("yyyy-MM-dd HH:mm:ss");
        QString insertQuery = QString("INSERT INTO repair_order (student_re_id, content, created_at) VALUES ('%1', '%2', '%3')")
                                  .arg(studentId)
                                  .arg(content)
                                  .arg(currentTimeStr);

        if (mysql_query(con, insertQuery.toLocal8Bit().constData())) {
            QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("提交报修失败: ") + QString::fromLocal8Bit(mysql_error(con)));
        } else {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("报修提交成功！"));
            updateRepairTable();
        }
    }
}

void MainWindow::onStudentIdEditingFinished()
{
    QString studentId = ui->usernameLineEdit->text();
    qDebug() << "Student ID editing finished: " << studentId;
    auto students = manager->get_student("id = '" + studentId.toStdString() + "'");
    displayStudentInfo(students);
    if (students.empty()) {
        QMessageBox::warning(this, QString::fromLocal8Bit("警告"), QString::fromLocal8Bit("无效的学生ID"));
    } else {
        displayDormitoryInfo(manager2->get_dormitory("student_id = '" + studentId.toStdString() + "'"));
    }
}

void MainWindow::on_replyRepairButton_clicked()
{
    QModelIndex index = ui->repairTableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择一条报修信息");
        return;
    }

    QString studentId = index.sibling(index.row(), 0).data().toString();
    QString content = index.sibling(index.row(), 1).data().toString();

    bool ok;
    QString reply = QInputDialog::getText(this, "回复报修", "请输入回复内容:", QLineEdit::Normal, "", &ok);
    if (ok && !reply.isEmpty()) {
        QString queryStr = QString("UPDATE repair_order SET reply = '%1' WHERE student_re_id = '%2' AND content = '%3'")
                               .arg(reply)
                               .arg(studentId)
                               .arg(content);

        if (mysql_query(con, queryStr.toLocal8Bit().constData())) {
            QMessageBox::critical(this, "错误", "回复报修失败: " + QString::fromLocal8Bit(mysql_error(con)));
        } else {
            QMessageBox::information(this, "成功", "回复报修成功！");
            updateRepairTable();
        }
    }
}

void MainWindow::on_addStudentButton_clicked() {
    qDebug() << "Add Student Button Clicked";

    AddStudentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        student stu;
        stu.id = dialog.getId().toLocal8Bit().constData();
        stu.name = dialog.getName().toLocal8Bit().constData();
        stu.gender = dialog.getGender().toLocal8Bit().constData();
        stu.birthdate = dialog.getBirthdate().toLocal8Bit().constData();
        stu.grade = dialog.getGrade().toLocal8Bit().constData();
        stu.major = dialog.getMajor().toLocal8Bit().constData();
        stu.classin = dialog.getClassIn().toLocal8Bit().constData();
        stu.phone = dialog.getPhone().toLocal8Bit().constData();
        stu.passwd = dialog.getPassword().toLocal8Bit().constData();

        if (manager->insert_student(stu)) {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("学生信息插入成功"));
            updateStudentTable();
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("失败"), QString::fromLocal8Bit("学生信息插入失败"));
        }
    }
}

void MainWindow::on_addDormitoryButton_clicked() {
    DormitoryManager *manager = DormitoryManager::getInstance();
    int lastDormitoryId = manager->getLastDormitoryId();
    int nextDormitoryId = lastDormitoryId + 1;

    AddDormitoryDialog dialog(this, nextDormitoryId);
    if (dialog.exec() == QDialog::Accepted) {
        Dormitory dorm;
        dorm.building = dialog.getBuilding().toLocal8Bit().constData();
        dorm.floor = dialog.getFloor().toLocal8Bit().constData();
        dorm.dorm_number = dialog.getDormNumber().toLocal8Bit().constData();
        dorm.type = dialog.getType().toLocal8Bit().constData();
        dorm.bed_id = dialog.getBedId().toLocal8Bit().constData();
        dorm.dormitory_id = std::to_string(nextDormitoryId);

        if (manager->insert_dormitory(dorm)) {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("宿舍添加成功！"));
            updateDormitoryTable();
        } else {
            QMessageBox::warning(this, QString::fromLocal8Bit("失败"), QString::fromLocal8Bit("宿舍添加失败！"));
        }
    }
}

void MainWindow::on_editButton_clicked(const QModelIndex &index) {
    QString student_id = ui->studentTableView->model()->index(index.row(), 0).data().toString();
    student stu;
    for (const auto &s : manager->get_student()) {
        if (s.id == student_id.toStdString()) {
            stu = s;
            break;
        }
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromLocal8Bit("学生信息编辑"));
    dialog.setModal(true);

    QFormLayout *formLayout = new QFormLayout(&dialog);
    formLayout->addRow(new QLabel(QString::fromLocal8Bit("学生信息编辑")));

    QLineEdit *idEdit = new QLineEdit(QString::fromLocal8Bit(stu.id.c_str()));
    idEdit->setReadOnly(true);
    QLineEdit *nameEdit = new QLineEdit(QString::fromLocal8Bit(stu.name.c_str()));
    QLineEdit *genderEdit = new QLineEdit(QString::fromLocal8Bit(stu.gender.c_str()));
    QLineEdit *birthdateEdit = new QLineEdit(QString::fromLocal8Bit(stu.birthdate.c_str()));
    QLineEdit *gradeEdit = new QLineEdit(QString::fromLocal8Bit(stu.grade.c_str()));
    QLineEdit *majorEdit = new QLineEdit(QString::fromLocal8Bit(stu.major.c_str()));
    QLineEdit *classinEdit = new QLineEdit(QString::fromLocal8Bit(stu.classin.c_str()));
    QLineEdit *phoneEdit = new QLineEdit(QString::fromLocal8Bit(stu.phone.c_str()));
    QLineEdit *passwordEdit = new QLineEdit(QString::fromLocal8Bit(stu.passwd.c_str()));

    formLayout->addRow(QString::fromLocal8Bit("学号:"), idEdit);
    formLayout->addRow(QString::fromLocal8Bit("姓名:"), nameEdit);
    formLayout->addRow(QString::fromLocal8Bit("性别:"), genderEdit);
    formLayout->addRow(QString::fromLocal8Bit("出生日期:"),birthdateEdit);
    formLayout->addRow(QString::fromLocal8Bit("年级:"), gradeEdit);
    formLayout->addRow(QString::fromLocal8Bit("专业:"), majorEdit);
    formLayout->addRow(QString::fromLocal8Bit("班级:"), classinEdit);
    formLayout->addRow(QString::fromLocal8Bit("电话:"), phoneEdit);
    formLayout->addRow(QString::fromLocal8Bit("密码:"), passwordEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        stu.name = nameEdit->text().toLocal8Bit().constData();
        stu.gender = genderEdit->text().toLocal8Bit().constData();
        stu.birthdate = birthdateEdit->text().toLocal8Bit().constData();
        stu.grade = gradeEdit->text().toLocal8Bit().constData();
        stu.major = majorEdit->text().toLocal8Bit().constData();
        stu.classin = classinEdit->text().toLocal8Bit().constData();
        stu.phone = phoneEdit->text().toLocal8Bit().constData();
        stu.passwd = passwordEdit->text().toLocal8Bit().constData();

        if (manager->update_student(stu)) {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("学生信息更新成功"));
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("失败"), QString::fromLocal8Bit("学生信息更新失败"));
        }
        updateStudentTable();
    }
}

void MainWindow::on_editDormitoryButton_clicked(const QModelIndex &index) {
    QString dormitory_id = index.siblingAtColumn(0).data(Qt::UserRole + 1).toString();

    int row = index.row();
    QString dorm_number = ui->dormitoryTableView->model()->index(row, 2).data().toString();
    QString type = ui->dormitoryTableView->model()->index(row, 3).data().toString();
    QString bed_id = ui->dormitoryTableView->model()->index(row, 4).data().toString();

    QDialog dialog(this);
    dialog.setWindowTitle(QString::fromLocal8Bit("宿舍信息编辑"));
    dialog.setModal(true);

    QFormLayout *formLayout = new QFormLayout(&dialog);

    QLineEdit *dormNumberEdit = new QLineEdit(dorm_number, &dialog);
    QLineEdit *typeEdit = new QLineEdit(type, &dialog);
    QLineEdit *bedIdEdit = new QLineEdit(bed_id, &dialog);

    formLayout->addRow(QString::fromLocal8Bit("宿舍号:"), dormNumberEdit);
    formLayout->addRow(QString::fromLocal8Bit("房间类型:"), typeEdit);
    formLayout->addRow(QString::fromLocal8Bit("床位号:"), bedIdEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, &dialog);
    formLayout->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        Dormitory dorm;
        dorm.dormitory_id = dormitory_id.toStdString();
        dorm.dorm_number = dormNumberEdit->text().toLocal8Bit().constData();
        dorm.type = typeEdit->text().toLocal8Bit().constData();
        dorm.bed_id = bedIdEdit->text().toLocal8Bit().constData();

        if (DormitoryManager::getInstance()->update_dormitory(dorm)) {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("宿舍信息更新成功"));
            updateDormitoryTable();
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("失败"), QString::fromLocal8Bit("宿舍信息更新失败"));
        }
    }
}

void MainWindow::on_deleteButton_clicked(const QModelIndex &index) {
    QString student_id = ui->studentTableView->model()->index(index.row(), 0).data().toString();

    if (QMessageBox::question(this, QString::fromLocal8Bit("确认删除"), QString::fromLocal8Bit("确定要删除该学生吗？")) == QMessageBox::Yes) {
        if (manager->delete_student(student_id.toStdString())) {
            QMessageBox::information(this, QString::fromLocal8Bit("删除成功"), QString::fromLocal8Bit("学生已成功删除"));
            updateStudentTable();
        } else {
            QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除学生时发生错误"));
        }
    }
}

void MainWindow::on_deleteDormitoryButton_clicked(const QModelIndex &index) {
    QString dormitory_id = index.siblingAtColumn(0).data(Qt::UserRole + 1).toString();

    qDebug() << "Attempting to delete dormitory with dormitory_id:" << dormitory_id;

    if (QMessageBox::question(this, QString::fromLocal8Bit("确认删除"), QString::fromLocal8Bit("确定要删除该宿舍吗？")) == QMessageBox::Yes) {
        if (DormitoryManager::getInstance()->delete_dormitory(dormitory_id.toStdString())) {
            QMessageBox::information(this, QString::fromLocal8Bit("删除成功"), QString::fromLocal8Bit("宿舍已成功删除"));
            updateDormitoryTable();
        } else {
            QMessageBox::warning(this, QString::fromLocal8Bit("删除失败"), QString::fromLocal8Bit("删除宿舍时发生错误"));
        }
    }
}

void MainWindow::updateStudentTable()
{
    auto students = manager->get_student();
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("学号")
                                                   << QString::fromLocal8Bit("姓名")
                                                   << QString::fromLocal8Bit("性别")
                                                   << QString::fromLocal8Bit("出生日期")
                                                   << QString::fromLocal8Bit("年级")
                                                   << QString::fromLocal8Bit("专业")
                                                   << QString::fromLocal8Bit("班级")
                                                   << QString::fromLocal8Bit("电话")
                                                   << QString::fromLocal8Bit("宿舍信息")
                                                   << QString::fromLocal8Bit("操作"));

    for (const auto &stu : students) {
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::fromLocal8Bit(stu.id.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.name.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.gender.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.birthdate.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.grade.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.major.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.classin.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.phone.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(stu.dormitory.c_str()));

        rowItems << new QStandardItem();

        model->appendRow(rowItems);
    }

    ui->studentTableView->setModel(model);
    ui->studentTableView->setItemDelegateForColumn(9, new ButtonDelegate(this));
    ui->studentTableView->horizontalHeader()->setStretchLastSection(true);
    auto delegate = static_cast<ButtonDelegate*>(ui->studentTableView->itemDelegateForColumn(9));
    connect(delegate, &ButtonDelegate::editButtonClicked, this, &MainWindow::on_editButton_clicked);
    connect(delegate, &ButtonDelegate::deleteButtonClicked, this, &MainWindow::on_deleteButton_clicked);
}

void MainWindow::updateCheckInTable()
{
    DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
    vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id IS NOT NULL");
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList()
                                     << QString::fromLocal8Bit("学号")
                                     << QString::fromLocal8Bit("姓名")
                                     << QString::fromLocal8Bit("楼栋号")
                                     << QString::fromLocal8Bit("楼层")
                                     << QString::fromLocal8Bit("宿舍号")
                                     << QString::fromLocal8Bit("房间类型")
                                     << QString::fromLocal8Bit("床位号"));
    for (const auto &dorm : dormitories) {
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::fromLocal8Bit(dorm.student_id.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.student_name.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.building.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.floor.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.dorm_number.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.type.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.bed_id.c_str()));
        for (auto &item : rowItems) {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
        }

        model->appendRow(rowItems);
    }
    ui->checkInTableView->setModel(model);
    ui->checkInTableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::updateDormitoryTable()
{
    auto dormitories = manager2->get_dormitory();
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList() << QString::fromLocal8Bit("楼栋号")
                                                   << QString::fromLocal8Bit("楼层")
                                                   << QString::fromLocal8Bit("宿舍号")
                                                   << QString::fromLocal8Bit("房间类型")
                                                   << QString::fromLocal8Bit("床位号")
                                                   << QString::fromLocal8Bit("学生学号")
                                                   << QString::fromLocal8Bit("学生姓名")
                                                   << QString::fromLocal8Bit("操作"));

    for (const auto &dorm : dormitories) {
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::fromLocal8Bit(dorm.building.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.floor.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.dorm_number.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.type.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.bed_id.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.student_id.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.student_name.c_str()));

        rowItems << new QStandardItem();

        model->appendRow(rowItems);
    }

    ui->dormitoryTableView->setModel(model);
    ui->dormitoryTableView->setItemDelegateForColumn(7, new ButtonDelegate(this));
    ui->dormitoryTableView->horizontalHeader()->setStretchLastSection(true);
    auto delegate = static_cast<ButtonDelegate*>(ui->dormitoryTableView->itemDelegateForColumn(7));
    connect(delegate, &ButtonDelegate::editButtonClicked, this, &MainWindow::on_editDormitoryButton_clicked);
    connect(delegate, &ButtonDelegate::deleteButtonClicked, this, &MainWindow::on_deleteDormitoryButton_clicked);
}

void MainWindow::displayStudentInfo(const vector<student>& students)
{
    if (students.empty()) {
        ui->studentInfoTextEdit->setPlainText(QString::fromLocal8Bit("未找到学生信息"));
        return;
    }

    const student& stu = students[0];
    QString studentInfo;
    studentInfo.append(QString::fromLocal8Bit("学号: %1\n").arg(QString::fromLocal8Bit(stu.id.c_str())));
    studentInfo.append(QString::fromLocal8Bit("姓名: %1\n").arg(QString::fromLocal8Bit(stu.name.c_str())));
    studentInfo.append(QString::fromLocal8Bit("性别: %1\n").arg(QString::fromLocal8Bit(stu.gender.c_str())));
    studentInfo.append(QString::fromLocal8Bit("出生日期: %1\n").arg(QString::fromLocal8Bit(stu.birthdate.c_str())));
    studentInfo.append(QString::fromLocal8Bit("年级: %1\n").arg(QString::fromLocal8Bit(stu.grade.c_str())));
    studentInfo.append(QString::fromLocal8Bit("专业: %1\n").arg(QString::fromLocal8Bit(stu.major.c_str())));
    studentInfo.append(QString::fromLocal8Bit("班级: %1\n").arg(QString::fromLocal8Bit(stu.classin.c_str())));
    studentInfo.append(QString::fromLocal8Bit("电话: %1\n").arg(QString::fromLocal8Bit(stu.phone.c_str())));

    ui->studentInfoTextEdit->setPlainText(studentInfo);
}

void MainWindow::displayDormitoryInfo(const vector<Dormitory>& dormitories)
{
    if (dormitories.empty()) {
        ui->dormitoryInfoTextEdit->setPlainText(QString::fromLocal8Bit("未找到宿舍信息"));
        return;
    }

    const Dormitory& dorm = dormitories[0];
    QString dormitoryInfo;
    dormitoryInfo.append(QString::fromLocal8Bit("楼栋号: %1\n").arg(QString::fromLocal8Bit(dorm.building.c_str())));
    dormitoryInfo.append(QString::fromLocal8Bit("楼层: %1\n").arg(QString::fromLocal8Bit(dorm.floor.c_str())));
    dormitoryInfo.append(QString::fromLocal8Bit("宿舍号: %1\n").arg(QString::fromLocal8Bit(dorm.dorm_number.c_str())));
    dormitoryInfo.append(QString::fromLocal8Bit("房间类型: %1\n").arg(QString::fromLocal8Bit(dorm.type.c_str())));
    dormitoryInfo.append(QString::fromLocal8Bit("床位号: %1\n").arg(QString::fromLocal8Bit(dorm.bed_id.c_str())));

    ui->dormitoryInfoTextEdit->setPlainText(dormitoryInfo);
}

void MainWindow::on_checkInButton_clicked()
{
    CheckInDialog dialog(con, this);
    if (dialog.exec() == QDialog::Accepted) {
        QString studentId = dialog.studentIdEdit->text();
        QString building = dialog.buildingEdit->text();
        QString floor = dialog.floorComboBox->currentText();
        QString type = dialog.typeComboBox->currentText();
        QString dormNumber = dialog.dormNumberComboBox->currentText();
        QString bedId = dialog.bedIdComboBox->currentText();

        DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
        vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id = '" + studentId.toStdString() + "'");
        if (!dormitories.empty()) {
            QMessageBox::information(this, QString::fromLocal8Bit("入住成功"), QString::fromLocal8Bit("该学生入住成功"));
            return;
        }

        dormitories = dormitoryManager->get_dormitory(
            "building = '" + building.toStdString() + "' AND "
                                                      "floor = '" + floor.toStdString() + "' AND "
                                    "dorm_number = '" + dormNumber.toStdString()+" AND bed_id = '" + bedId.toStdString() + "'");

        if (dormitories.empty()) {
            QMessageBox::warning(this, QString::fromLocal8Bit("入住失败"), QString::fromLocal8Bit("未找到指定的床位信息"));
            return;
        }

        Dormitory &dorm = dormitories[0];
        dorm.student_id = studentId.toStdString();

        if (dormitoryManager->update_dormitory(dorm)) {
            QMessageBox::information(this, QString::fromLocal8Bit("入住成功"), QString::fromLocal8Bit("学生入住成功"));
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("入住失败"), QString::fromLocal8Bit("更新宿舍信息失败"));
        }

        updateCheckInTable();
    }
}

void MainWindow::on_checkOutButton_clicked() {
    CheckOutDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QString studentId = dialog.getStudentId();

        DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
        vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id = '" + studentId.toStdString() + "'");

        if (dormitories.empty()) {
            QMessageBox::warning(this, QString::fromLocal8Bit("退宿失败"), QString::fromLocal8Bit("该学生未入住"));
            return;
        }

        Dormitory &dorm = dormitories[0];
        dorm.student_id = "";
        if (dormitoryManager->clear_student_id(dorm.dormitory_id)) {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("退宿成功"));
            updateCheckOutTable();
        } else {
            QMessageBox::critical(this, QString::fromLocal8Bit("失败"), QString::fromLocal8Bit("退宿失败"));
        }
    }
}

void MainWindow::updateCheckOutTable() {
    DormitoryManager *dormitoryManager = DormitoryManager::getInstance();
    vector<Dormitory> dormitories = dormitoryManager->get_dormitory("student_id IS NOT NULL");
    QStandardItemModel *model = new QStandardItemModel();
    model->setHorizontalHeaderLabels(QStringList()
                                     << QString::fromLocal8Bit("学号")
                                     << QString::fromLocal8Bit("姓名")
                                     << QString::fromLocal8Bit("楼栋号")
                                     << QString::fromLocal8Bit("楼层")
                                     << QString::fromLocal8Bit("宿舍号")
                                     << QString::fromLocal8Bit("房间类型")
                                     << QString::fromLocal8Bit("床位号"));
    for (const auto &dorm : dormitories) {
        QList<QStandardItem *> rowItems;
        rowItems << new QStandardItem(QString::fromLocal8Bit(dorm.student_id.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.student_name.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.building.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.floor.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.dorm_number.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.type.c_str()))
                 << new QStandardItem(QString::fromLocal8Bit(dorm.bed_id.c_str()));
        for (auto &item : rowItems) {
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            item->setTextAlignment(Qt::AlignCenter);
        }

        model->appendRow(rowItems);
    }
    ui->checkOutTableView->setModel(model);
    ui->checkOutTableView->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::on_viewAnnouncementButton_clicked()
{
    QStringList announcements;
    QString selectQuery = "SELECT content, created_at FROM announcement ORDER BY created_at DESC";

    if (mysql_query(con, selectQuery.toLocal8Bit().constData())) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取公告列表失败: %1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    MYSQL_RES *result = mysql_store_result(con);
    if (result == nullptr) {
        QMessageBox::critical(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("获取公告结果失败: %1").arg(QString::fromLocal8Bit(mysql_error(con))));
        return;
    }

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) {
        QString announcement = QString::fromLocal8Bit(row[0]);
        QString timestamp = QString::fromLocal8Bit(row[1]);
        QString displayText = QString("%1 [%2]").arg(announcement).arg(timestamp);
        announcements.append(displayText);
    }

    mysql_free_result(result);

    announcementDialog->setAnnouncements(announcements);
    announcementDialog->show();
}

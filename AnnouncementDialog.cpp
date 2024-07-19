#include "AnnouncementDialog.h"
#include "ui_AnnouncementDialog.h"

AnnouncementDialog::AnnouncementDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AnnouncementDialog)
{
    ui->setupUi(this);
}

AnnouncementDialog::~AnnouncementDialog()
{
    delete ui;
}

void AnnouncementDialog::setAnnouncements(const QStringList &announcements)
{
    ui->announcementListWidget->clear();
    ui->announcementListWidget->addItems(announcements);
}

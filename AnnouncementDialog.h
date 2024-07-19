#ifndef ANNOUNCEMENTDIALOG_H
#define ANNOUNCEMENTDIALOG_H

#include <QDialog>
#include <QStringList>

namespace Ui {
class AnnouncementDialog;
}

class AnnouncementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AnnouncementDialog(QWidget *parent = nullptr);
    ~AnnouncementDialog();

    void setAnnouncements(const QStringList &announcements);

private:
    Ui::AnnouncementDialog *ui;
};

#endif // ANNOUNCEMENTDIALOG_H

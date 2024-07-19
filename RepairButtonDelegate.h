#ifndef REPAIRBUTTONDELEGATE_H
#define REPAIRBUTTONDELEGATE_H

#include <QStyledItemDelegate>
#include <QPushButton>
#include <QModelIndex>

class RepairButtonDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    RepairButtonDelegate(QObject *parent = nullptr);
    ~RepairButtonDelegate();

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void viewReplyButtonClicked(const QModelIndex &index);
    void deleteButtonClicked(const QModelIndex &index);
};

#endif // REPAIRBUTTONDELEGATE_H

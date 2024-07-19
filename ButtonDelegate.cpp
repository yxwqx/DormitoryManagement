#include "ButtonDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

ButtonDelegate::ButtonDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

ButtonDelegate::~ButtonDelegate()
{
}

void ButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton editButton;
    editButton.rect = QRect(option.rect.left(), option.rect.top(), option.rect.width() / 2, option.rect.height());
    editButton.text = "编辑";
    editButton.state = QStyle::State_Enabled;

    QStyleOptionButton deleteButton;
    deleteButton.rect = QRect(option.rect.left() + option.rect.width() / 2, option.rect.top(), option.rect.width() / 2, option.rect.height());
    deleteButton.text = "删除";
    deleteButton.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &editButton, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &deleteButton, painter);
}

bool ButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            int clickX = mouseEvent->pos().x();
            int clickY = mouseEvent->pos().y();

            QStyleOptionButton editButton;
            editButton.rect = QRect(option.rect.left(), option.rect.top(), option.rect.width() / 2, option.rect.height());

            QStyleOptionButton deleteButton;
            deleteButton.rect = QRect(option.rect.left() + option.rect.width() / 2, option.rect.top(), option.rect.width() / 2, option.rect.height());

            if (editButton.rect.contains(clickX, clickY)) {
                emit editButtonClicked(index);
                return true;
            } else if (deleteButton.rect.contains(clickX, clickY)) {
                emit deleteButtonClicked(index);
                return true;
            }
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

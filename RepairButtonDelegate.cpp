#include "RepairButtonDelegate.h"
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

RepairButtonDelegate::RepairButtonDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

RepairButtonDelegate::~RepairButtonDelegate()
{
}

void RepairButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &/*index*/) const
{
    QStyleOptionButton viewReplyButton;
    viewReplyButton.rect = QRect(option.rect.left(), option.rect.top(), option.rect.width() / 2, option.rect.height());
    viewReplyButton.text = "²é¿´/»Ø¸´";
    viewReplyButton.state = QStyle::State_Enabled;

    QStyleOptionButton deleteButton;
    deleteButton.rect = QRect(option.rect.left() + option.rect.width() / 2, option.rect.top(), option.rect.width() / 2, option.rect.height());
    deleteButton.text = "É¾³ý";
    deleteButton.state = QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_PushButton, &viewReplyButton, painter);
    QApplication::style()->drawControl(QStyle::CE_PushButton, &deleteButton, painter);
}

bool RepairButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QRect viewReplyRect = QRect(option.rect.left(), option.rect.top(), option.rect.width() / 2, option.rect.height());
        QRect deleteRect = QRect(option.rect.left() + option.rect.width() / 2, option.rect.top(), option.rect.width() / 2, option.rect.height());

        if (viewReplyRect.contains(mouseEvent->pos())) {
            emit viewReplyButtonClicked(index);
        } else if (deleteRect.contains(mouseEvent->pos())) {
            emit deleteButtonClicked(index);
        }
    }

    return QStyledItemDelegate::editorEvent(event, model, option, index);
}

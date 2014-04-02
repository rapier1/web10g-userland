#ifndef ESTATSITEMDELEGATE_HPP
#define ESTATSITEMDELEGATE_HPP


#include <QStyledItemDelegate>


class QModelIndex;
class QPainter;
class QStyleOptionViewItem;


class EstatsItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit EstatsItemDelegate(QObject *parent=0)
        : QStyledItemDelegate(parent) {}
};

#endif // ESTATSITEMDELEGATE_HPP

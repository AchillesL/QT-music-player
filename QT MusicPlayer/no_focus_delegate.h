//���ܣ�����qtableWidget������ֵ����
#ifndef NO_FOCUS_DELEGATE_H
#define NO_FOCUS_DELEGATE_H
#include <QtGui>
class QLineDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:

    QLineDelegate();
protected:
    void paint(QPainter* painter,const QStyleOptionViewItem& option,const QModelIndex& index) const;
private:
    QPen         pen;
    QTableView*  view;
};
#endif // NO_FOCUS_DELEGATE_H

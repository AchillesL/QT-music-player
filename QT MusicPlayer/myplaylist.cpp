#include "myplaylist.h"
#include <QContextMenuEvent>
#include <QMenu>


MyPlaylist::MyPlaylist(QWidget *parent) :
    QTableWidget(parent)
{

    resize(325, 350);

    setRowCount(0);             //��ʼ������Ϊ0
    setColumnCount(3);          //��ʼ������Ϊ1

    //���õ�һ����ǩ
    QStringList list;
    list << tr("����") << tr("����") << tr("����");
    setHorizontalHeaderLabels(list);

    setSelectionMode(QAbstractItemView::SingleSelection);//����ֻ��ѡ����
    setSelectionBehavior(QAbstractItemView::SelectRows);

    setShowGrid(false);//���ò���ʾ����


}


void MyPlaylist::clear_play_list()
{
    while(rowCount())
        removeRow(0);
    emit play_list_clean();//ɾ����󣬷�����ճɹ��ź�

}
void MyPlaylist::add_File_slot()
{
    emit add_File_signal();
}


void MyPlaylist::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    menu.addAction(tr("��Ӹ���"),this,SLOT(add_File_slot()));
    if(this->rowCount()!= 0)
    {
        menu.addAction(tr("����б�"), this, SLOT(clear_play_list()));//����ֱ��������ָ���ۺ���
    }

    menu.setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                       "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                       "QMenu::item:selected:!enabled{background:transparent;}"
                       "QMenu::item:selected:enabled{background:lightgray;color:white;}");
    menu.exec(event->globalPos());//�������ָ���ȫ��λ��


}


void MyPlaylist::closeEvent(QCloseEvent *event)
{
    if(isVisible()) {
        hide();
        event->ignore();//������ձ�־
    }
}



#include "mylrc.h"
#include <QPainter>
#include <QTimer>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>


MyLrc::MyLrc(QWidget *parent) :
    QLabel(parent)
{
    //FramelessWindowHintΪ�ޱ߽�Ĵ���
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setText(tr("M����"));
    // �̶���ʾ�����С
    setMaximumSize(800, 60);
    setMinimumSize(800, 60);

    //��ʵ����Խ������
    linear_gradient.setStart(0, 10);//�����������
    linear_gradient.setFinalStop(0, 40);//�����յ�����
    //��һ�������յ����꣬��������������������ԣ����ձ������м���
    linear_gradient.setColorAt(0.1, QColor(14, 179, 255));
    linear_gradient.setColorAt(0.5, QColor(114, 232, 255));
    linear_gradient.setColorAt(0.9, QColor(14, 179, 255));

    // ���ֵ����Խ������
    mask_linear_gradient.setStart(0, 10);
    mask_linear_gradient.setFinalStop(0, 40);
    mask_linear_gradient.setColorAt(0.1, QColor(222, 54, 4));
    mask_linear_gradient.setColorAt(0.5, QColor(255, 72, 16));
    mask_linear_gradient.setColorAt(0.9, QColor(222, 54, 4));

    // ��������
    font.setFamily("Times New Roman");
    font.setBold(true);
    font.setPointSize(30);

    // ���ö�ʱ��
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    lrc_mask_width = 0;
    lrc_mask_width_interval = 0;
}


// �������֣���Ҫָ����ǰ��ʿ�ʼ�����֮���ʱ����
void MyLrc::start_lrc_mask(qint64 intervaltime)
{
    // ��������ÿ��30�������һ�����ֵĿ�ȣ���Ϊ�������̫Ƶ��
    // ������CPUռ���ʣ������ʱ����̫���򶯻�Ч���Ͳ�������
    qreal count = intervaltime / 30;
    // ��ȡ����ÿ����Ҫ���ӵĿ�ȣ������800�ǲ����Ĺ̶����
    lrc_mask_width_interval = 800 / count;
    lrc_mask_width = 0;
    timer->start(30);
}

void MyLrc::stop_lrc_mask()
{
    timer->stop();
    lrc_mask_width = 0;
    update();
}


void MyLrc::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setFont(font);

    // �Ȼ��Ƶײ����֣���Ϊ��Ӱ��������ʹ��ʾЧ�������������Ҹ����ʸ�
    painter.setPen(QColor(0, 0, 0, 200));
    painter.drawText(1, 1, 800, 60, Qt::AlignLeft, text());//�����

    // ����������ƽ�������
    painter.setPen(QPen(linear_gradient, 0));
    painter.drawText(0, 0, 800, 60, Qt::AlignLeft, text());

    // ���ø������
    painter.setPen(QPen(mask_linear_gradient, 0));
    painter.drawText(0, 0, lrc_mask_width, 60, Qt::AlignLeft, text());

}

//�������
void MyLrc::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
        offset = event->globalPos() - frameGeometry().topLeft();
}


void MyLrc::mouseMoveEvent(QMouseEvent *event)
{
    //�ƶ���굽�����ʱ������ʾ����
    //event->buttons()��������������ͣ���Ϊ������л����һ�
    //�������������ʾ�����
    if (event->buttons() & Qt::LeftButton) {
        setCursor(Qt::PointingHandCursor);
        //ʵ���ƶ�����
        move(event->globalPos() - offset);
       }
}

//�һ��¼�
void MyLrc::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                       "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                       "QMenu::item:selected:!enabled{background:transparent;}"
                       "QMenu::item:selected:enabled{background:lightgray;color:white;}"
);
    menu.addAction(tr("����"), this, SLOT(myhide_slot()));
    menu.exec(event->globalPos());//globalPos()Ϊ��ǰ����λ������
}


void MyLrc::timeout()
{
    //ÿ��һ�ι̶���ʱ�����ֵĳ��Ⱦ�����һ��
    lrc_mask_width += lrc_mask_width_interval;
    update();//����widget�����ǲ��������ػ棬���ǰ���һ��Paint�¼�����������ѭ��ʱ��ϵͳ���ػ�
}
void MyLrc::myhide_slot()
{
    emit myhide_signal(false);
    this->hide();

}


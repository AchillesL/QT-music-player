#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //��ȡϵͳ���룬������ֲ���������
        QTextCodec *codec = QTextCodec::codecForName("System");

        //���úͶԱ����ļ�ϵͳ��дʱ���Ĭ�ϱ����ʽ
        QTextCodec::setCodecForLocale(codec);

        //���ô���tr����ʱ��Ĭ���ַ�������
        QTextCodec::setCodecForTr(codec);

        //�����ַ���������QByteArray����QString����ʱʹ�õ�һ�ֱ��뷽ʽ
        QTextCodec::setCodecForCStrings(codec);
    MainWindow w;
    w.show();

    return a.exec();
}

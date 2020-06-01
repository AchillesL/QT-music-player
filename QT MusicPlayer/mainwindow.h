#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGui>
#include <phonon>
#include "myplaylist.h"
#include "MySystemTray.h"

class MyLrc;
class QPoint;
class QToolButton;
class QSystemTrayIcon;
class QAction;
class QMenu;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //�����Ӧ�¼�
    void mousePressEvent(QMouseEvent *event);           //��갴���¼�
    void mouseReleaseEvent(QMouseEvent *event);         //����ͷ��¼�
    void mouseMoveEvent(QMouseEvent *event);            //����ƶ��¼�

    void MyWindowStyle();                               //�����������������
    void MyWindowButtonStyle();                         //�����尴ť��������
    void MyTrayIcon();                                  //ϵͳ��������

    void initTableWidget();
    void paintEvent(QPaintEvent *e);
    void initRightMenu();                                           //��ʼ���Ҽ��˵�
    void contextMenuEvent(QContextMenuEvent *event);                //�Ҽ��˵�

    void keyPressEvent(QKeyEvent *e);
    void initEventFilter();                                         //��ʼ���¼�������
    bool eventFilter(QObject *obj, QEvent *e);                      //�¼�������ʵ��

    void initMusic();                                               //��ʼ����Ƶ��Ϣ
    void initFlag();                                                //��ʼ�����

    void SkipBackward();                                            //��һ��
    void SkipForward();                                             //��һ��

    void readfile();
    void writefile();

    void setPlaymodebuttontext(int mode);
    void setWindowHeight(int flag);
signals:
    void set_tray_songnamelabel(QString s);
    void re_tray_songnamelabel();
    void vol_change_signal(qreal vol);
    void changethetraystate(Phonon::State new_state);





private:
    Ui::MainWindow *ui;

    QPoint move_point;                  //����ƶ��ľ���
    bool mouse_press;                   //����Ƿ���

    QSystemTrayIcon *trayIcon;          //ϵͳ����

    //QAction *loadAction;                //ϵͳ���̵�½��
    QAction *quitAction;                //ϵͳ�����˳���
    QMenu   *trayIconMenu;              //ϵͳ���̲˵�
    QMenu   *rightmenu;                 //�Ҽ��˵�
    MyPlaylist *playlist;
    SystemTray *mytrayIcon;
    int flag;                           //��ǵ�ǰ����ͼ��״̬
    int playflag ;                      //��ǵ�ǰ����״̬
    qreal vol;

    int songNo;
    int playsongNO;
    int playmode;                       //��ǲ���ģʽ
    int firsttime;                      //��ǵ�һ������
    int thelistopenflag;

    MyLrc *lrc;
    QMap<qint64, QString> lrc_map;
    void resolve_lrc(const QString &source_file_name);

    //ý����
    Phonon::SeekSlider *seekSlider;
    Phonon::MediaObject *mediaObject;
    Phonon::MediaObject *metaInformationResolver;
    Phonon::AudioOutput *audioOutput;
    Phonon::VolumeSlider *volumeSlider;
    Phonon::AudioOutput *metaInformationResolveraudioOutput;
    QList<Phonon::MediaSource> sources;

private slots:



    void on_toolButton_2_clicked();
    void on_toolButton_clicked();
    void on_checkBox_lrc_stateChanged(int );
    void on_checkBox_list_stateChanged(int );
   // void on_checkBox_list_clicked();
    void on_nextButton_clicked();
  //  void on_checkBox_list_stateChanged(int );
    void on_iconbutton_clicked();

    void on_playmode_clicked();
    void on_playButton_clicked();
    void on_styleButton_clicked();
    void on_lastButton_clicked();
    void myclose();
    void addfile();                                                 //��������ļ�


    void updateTime(qint64 time);

    void setSilent();                                               //����
    void iconIsActived(QSystemTrayIcon::ActivationReason reason);   //���̵����Ϣ
    void AboutToFinish();
    void StateChanged(Phonon::State new_state, Phonon::State old_state);
    void MetaStateChanged(Phonon::State new_state, Phonon::State old_state);
    void SourceChanged(const Phonon::MediaSource &source);
    void ClearSources();
    void TableClicked(QTableWidgetItem* item);
    void traytochangvol(qreal vol);
    void volchange_slot(qreal qvol);
    void traytoclearlist_slot();

    void addvol_slot();
    void devvol_slot();
    void showThisWindow();

    void SetLrcShown();

    void playmode_sc_slot();
    void playmode_sp_slot();
    void playmode_lp_slot();
    void playmode_lc_slot();

    void showAuthor();
    void showKey();

};

#endif // MAINWINDOW_H

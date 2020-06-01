//�Զ���ϵͳ������
#ifndef MYSYSTEMTRAY_H
#define MYSYSTEMTRAY_H

#include <QtGui>
#include <phonon>

class SystemTray : public QSystemTrayIcon
{
    Q_OBJECT
public:

    explicit SystemTray(QWidget *parent = 0);
    ~SystemTray();
    void translateLanguage();
    void createActions();
    void createPlayAction();
    void addActions();

private:
    QMenu *pop_menu;                    //�˵�
    QAction *show_lrc_action;           //��ʾ���
    QAction *show_action ;              //��ʾ������
    QAction *addfile_action;            //����ļ�
    QAction *clearlist_action;          //����б�
    QAction *quit_action;               //�˳�
    QWidget *play_widget;
    QWidgetAction *play_widget_action ; //���Ű�ť��widget
    QWidget *vol_widget;
    QWidgetAction *vol_slider_widgetAction;   //����������widget

    QPushButton *play_button ;          //���Ű�ť
    QPushButton *backward_button ;      //��һ��
    QPushButton *forward_button ;       //��һ��
    QLabel *music_name_label ;          //��ǰ��������
    QSlider *vol_slider;                //��������
    QToolButton *vol_button;            //������ť

    int no_vol_flag;                    //��ǵ�ǰ�Ƿ���


signals:
    void show_action_signal();
    void play_signal();
    void backward_signal();
    void forward_signal();
    void show_lrc_signal();
    void quit_signal();
    void no_vol_signal();
    void changevol_signal(qreal vol);
    void addfile_signal();
    void clearlist_signal();
  //  void showMain_signal();

public slots:
    void show_action_slot();
    void play_slot();
    void backward_slot();
    void forward_slot();
    void show_lrc_slot();
    void quit_slot();
    void set_song_name(QString s);
    void re_song_name();
    void no_vol_slots();
    void changevol_slot(int vol);
    void changevolSlider_slot(qreal vol);
    void addfile_slot();
    void clearlist_slot();
    void state(Phonon::State state);
    //void showMain_slot();

};

#endif // MYSYSTEMTRAY_H

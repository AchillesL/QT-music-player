#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QColorDialog>
#include <QFileDialog>
#include <QActionGroup>
#include <phonon>
#include <QFileInfo>
#include <QDebug>
#include "myplaylist.h"
#include "no_focus_delegate.h"
#include <QUrl>
#include <QList>
#include "mylrc.h"
//#include <QKeySequence>
#include <QAction>
#include <QFile>
MainWindow::MainWindow(QWidget *parent) :
        QMainWindow(parent),
        ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MyWindowStyle();
    MyWindowButtonStyle();
    MyTrayIcon();
    initMusic();
    initEventFilter();
    initTableWidget();
    initFlag();

    songNo = 0;

    lrc = new MyLrc(this);

    readfile();
    connect(lrc,SIGNAL(myhide_signal(bool)),ui->checkBox_lrc,SLOT(setChecked(bool)));

    //�ı�������ʾ�ĸ�����
    connect(this,SIGNAL(set_tray_songnamelabel(QString)),mytrayIcon,SLOT(set_song_name(QString)));
    //���ñ���
    connect(this,SIGNAL(re_tray_songnamelabel()),mytrayIcon,SLOT(re_song_name()));
    //�����ı�ʱ�����ź�
    connect(this,SIGNAL(vol_change_signal(qreal)),mytrayIcon,SLOT(changevolSlider_slot(qreal)));
    //ý�岥��״̬�ı��Ƿ����ź�
    connect(this,SIGNAL(changethetraystate(Phonon::State)),mytrayIcon,SLOT(state(Phonon::State)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


//��д����¼�,��Ϊ����������Ϊ�ޱ߿�ģʽ������д�����Ӧ�¼���ʵ�ִ�����϶�
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mouse_press = true;
        //�������ڴ����λ��
        move_point = event->pos();
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    //��������������
    if(mouse_press)
    {
        //����������Ļ��λ��
        QPoint move_pos = event->globalPos();

        //�ƶ�������λ��
        this->move(move_pos - move_point);
    }
}
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    //�������Ϊδ������
    mouse_press = false;
}

void MainWindow::myclose()                  //����ر�
{
    //int msg = QMessageBox::warning(NULL, "warning", "Content", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
    //if(msg == QMessageBox::No) return;
    //����ڲ������֣���ֹͣ
    if(mediaObject->state() == Phonon::PlayingState)
    {
        mediaObject->stop();

    }
    writefile();
    //���浭��
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation, SIGNAL(finished()), this, SLOT(close()));
}
void MainWindow::MyWindowStyle()            //���ô�������
{
    //���ô���Ϊ�̶���С���߶�190Ϊ����ʾ�б�״̬
    this->setFixedSize(this->width(),190);

    //����ͼ��
    QIcon icon = QIcon(":/images/ico.png");
    this->setWindowIcon(icon);

    //���ý����ޱ߿�λ��״̬����
    this->setWindowFlags(Qt::FramelessWindowHint );
    this->setMouseTracking(true);


    //�����ܽ����϶�
    this->setAcceptDrops(true);

    //���ó�ʼ������ɫ������ɫ
    QPalette p;
    p.setBrush(this->backgroundRole(),QBrush(QColor(0,0,127,255)));
    this->setPalette(p);
    this->setAutoFillBackground(true);

    ui->songtotal->setText("");
    ui->songnum->setText("");

    playmode = 3;

    //���潥�����
    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
}
void MainWindow::MyWindowButtonStyle()      //���ô�������С�����رհ�ť����
{
    ui->playButton->setCursor(Qt::PointingHandCursor);
    ui->nextButton->setCursor(Qt::PointingHandCursor);
    ui->lastButton->setCursor(Qt::PointingHandCursor);
    //��ȡ��С�����رհ�ť
   // QPixmap minPix = style()->standardPixmap(QStyle::SP_TitleBarShadeButton);
    //QPixmap closePix = style()->standardPixmap(QStyle::SP_TitleBarCloseButton);

    //������С�����رհ�ťͼ��
    //ui->minButton->setIcon(minPix);
    //ui->closeButton->setIcon(closePix);

    //������С�����رհ�ť�ڽ����λ��
    ui->minButton->setGeometry(this->width()-46,8,20,20);
    ui->closeButton->setGeometry(this->width()-25,5,20,20);

    //������С�����رհ�ť�������״
    ui->minButton->setCursor(Qt::PointingHandCursor);
    ui->closeButton->setCursor(Qt::PointingHandCursor);


    //������С�����رհ�ť����ʽ
    ui->minButton->setStyleSheet("background-color:transparent;");
    ui->closeButton->setStyleSheet("background-color:transparent;");


    //��С�����رհ�ť��Ӧ�¼�
    connect(ui->minButton,SIGNAL(clicked()),SLOT(showMinimized()));
    connect(ui->closeButton,SIGNAL(clicked()),SLOT(myclose()));


}
void MainWindow::initTableWidget()      //��ʼ���б�
{
    playlist = new MyPlaylist(this);
    playlist->move(20,200);                                     //����λ��
    playlist->setFrameShape(QFrame::NoFrame);                   //�����ޱ߿�
    playlist->horizontalHeader()->setStretchLastSection(true);  //��ͷ����
    playlist->horizontalHeader()->resizeSection(0,200);         //���ñ�ͷ��һ����
    playlist->horizontalHeader()->resizeSection(1,70);          //���ñ�ͷ�ڶ�����
    playlist->horizontalHeader()->setClickable(false);          //���ñ�ͷ���ܵ��
    playlist->setItemDelegate(new QLineDelegate());             //����ѡ����ʱ����ʾ���

    QHeaderView *headerView = playlist->verticalHeader();
    headerView->setHidden(true);                                //���ش�ֱ��ͷ

   // playlist->setStyleSheet("selection-background-color: rgb(152, 171, 255);"
   //                         "selection-color:rgb(0,0,0);");
    //��˫���б������ʱ����Ӧ
    connect(playlist,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(TableClicked(QTableWidgetItem*)));
    //����б�ʱ��Ӧ
    connect(playlist, SIGNAL(play_list_clean()), this, SLOT(ClearSources()));
    //����ļ�
    connect(playlist,SIGNAL(add_File_signal()),this,SLOT(addfile()));
    //connect(playlist,SIGNAL(cellEntered(int,int)))

}

void MainWindow::initMusic()                //��ʼ��ý����󣬲�������ؿؼ�����
{
    //����ý�����
    audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    metaInformationResolveraudioOutput =  new Phonon::AudioOutput(Phonon::MusicCategory, this);

    mediaObject = new Phonon::MediaObject(this);
    metaInformationResolver = new Phonon::MediaObject(this);
    Phonon::createPath(metaInformationResolver, metaInformationResolveraudioOutput);



    //��Դ�ͽ�����
    Phonon::createPath(mediaObject, audioOutput);

    audioOutput->setVolume(0.9);
    //����tick�źż�Ϸ���ʱ�䣬1000���룬���ڸ���ʱ��label
    mediaObject->setTickInterval(1000);

    //���������������������
    seekSlider = new Phonon::SeekSlider(mediaObject);
    seekSlider->setCursor(Qt::PointingHandCursor);
    ui->verticalLayout_slider->addWidget(seekSlider);
    seekSlider->setStyleSheet("QSlider::groove:horizontal{border:0px;height:4px;}"
                              "QSlider::sub-page:horizontal{background:white;}"
                              "QSlider::add-page:horizontal{background:lightgray;}"
                              "QSlider::handle:horizontal{background:white;width:10px;border-radius:5px;margin:-3px 0px -3px 0px;}");
    //�����������������������
    volumeSlider = new Phonon::VolumeSlider();
    volumeSlider->setCursor(Qt::PointingHandCursor);
    ui->verticalLayout->addWidget(volumeSlider);
    volumeSlider->setAudioOutput(audioOutput);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal{border:0px;height:4px;}"
                                "QSlider::sub-page:horizontal{background:white;}"
                                "QSlider::add-page:horizontal{background:lightgray;}"
                                "QSlider::handle:horizontal{background:white;width:10px;border-radius:5px;margin:-3px 0px -3px 0px;}");



    //����ý������tick�ź������²���ʱ�����ʾ
    connect(mediaObject,SIGNAL(tick(qint64)),this,SLOT(updateTime(qint64)));
    //��ǰ�����������´����Ķ���
    connect(mediaObject,SIGNAL(aboutToFinish()),this,SLOT(AboutToFinish()));
    //��ý��״̬�����ı�ʱ����������
    connect(mediaObject,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(StateChanged(Phonon::State,Phonon::State)));
    connect(metaInformationResolver,SIGNAL(stateChanged(Phonon::State,Phonon::State)),this,SLOT(MetaStateChanged(Phonon::State,Phonon::State)));
    connect(mediaObject,SIGNAL(currentSourceChanged(Phonon::MediaSource)),this,SLOT(SourceChanged(Phonon::MediaSource)));

    //�����������ı�ʱ����
    connect(audioOutput,SIGNAL(volumeChanged(qreal)),this,SLOT(volchange_slot(qreal)));
}
void MainWindow::initFlag()                 //��ʼ�����
{
    flag = 0;
    playflag = 0;
    firsttime = 0;
}

void MainWindow::MyTrayIcon()               //ʵ�����̹���
{

    QString title = tr("M����");
    QString text = tr("���㲻һ������������");

    mytrayIcon = new SystemTray(this);

    mytrayIcon->show();
    mytrayIcon->showMessage(title,text,QSystemTrayIcon::Information,4000);

    connect(mytrayIcon,SIGNAL(no_vol_signal()),this,SLOT(setSilent()));
    connect(mytrayIcon,SIGNAL(play_signal()),this,SLOT(on_playButton_clicked()));
    connect(mytrayIcon,SIGNAL(forward_signal()),this,SLOT(on_nextButton_clicked()));
    connect(mytrayIcon,SIGNAL(backward_signal()),this,SLOT(on_lastButton_clicked()));
    connect(mytrayIcon,SIGNAL(quit_signal()),this,SLOT(myclose()));
    connect(mytrayIcon,SIGNAL(changevol_signal(qreal)),this,SLOT(traytochangvol(qreal)));
    connect(mytrayIcon,SIGNAL(addfile_signal()),this,SLOT(addfile()));
    connect(mytrayIcon,SIGNAL(clearlist_signal()),this,SLOT(traytoclearlist_slot()));
    connect(mytrayIcon,SIGNAL(show_action_signal()),this,SLOT(showThisWindow()));
    connect(mytrayIcon,SIGNAL(show_lrc_signal()),this,SLOT(SetLrcShown()));
    //���̵����Ӧ
    connect(mytrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(iconIsActived(QSystemTrayIcon::ActivationReason)));


}
void MainWindow::iconIsActived(QSystemTrayIcon::ActivationReason reason)    //���̵����Ӧ�¼�
{
    switch(reason)
    {
    case QSystemTrayIcon::Trigger:          //����
    case QSystemTrayIcon::DoubleClick:      //˫��
        this->showNormal();
        this->raise();
        break;
    default:
        break;
    }
}
void MainWindow::initEventFilter()          //Ϊ�ؼ�ע���¼�������
{
    ui->playButton->installEventFilter(this);        //���Ű�ť
    ui->nextButton->installEventFilter(this);        //��һ�װ�ť
    ui->lastButton->installEventFilter(this);        //��һ�װ�ť
    ui->styleButton->installEventFilter(this);       //������ť
    this->installEventFilter(this);                  //������
}
bool MainWindow::eventFilter(QObject *obj, QEvent *e)//�¼���������ʵ�ֲ���/��ͣ��ť����/��һ�װ�ť��������ť��ͼ���л���
{

    if(obj == this)
    {
        QString s;
        if(e->type() == QEvent::DragEnter)
        {
            QDragEnterEvent *dee = dynamic_cast<QDragEnterEvent *>(e);
                      dee->acceptProposedAction();
                        return true;
        }
        else if(e->type() == QEvent::Drop)
        {
            //��������ļ������б�
            QDropEvent *de = dynamic_cast<QDropEvent *>(e);
            QList<QUrl> urls = de->mimeData()->urls();
            int index = sources.size();

            if (urls.isEmpty()) { return true; }
            for (int i = 0; i < urls.size(); ++i) {

                s = urls.at(i).toString().replace("file:///","");
                Phonon::MediaSource source(s);
                sources.append(source);

             }
            if(!sources.isEmpty())
            {
                //���ý��Դ�б�Ϊ�գ����¼���ĵ�һ��ý��Դ��Ϊ��ǰý��Դ
                metaInformationResolver->setCurrentSource(sources.at(index));
            }

        }
    }

    if(obj == ui->playButton)               //��ǰ����Ϊ���Ű�ť
    {
        if(e->type() == QEvent::Enter)      //�����밴ťʱ����
        {
            if(flag == 1)                   //flagΪ1����ͣ״̬��Ϊ0������״̬
            {
                //�л���ť����ͼ
                ui->playButton->setIcon(QPixmap(":/images/play1.png"));
            }
            if(flag == 0)
            {
                ui->playButton->setIcon(QPixmap(":/images/play4.png"));
            }

        }
        if(e->type() == QEvent::Leave)              //����뿪��ťʱ����
        {
            if(flag == 1)
            {
                ui->playButton->setIcon(QPixmap(":/images/play2.png"));
            }
            if(flag == 0)
            {
                ui->playButton->setIcon(QPixmap(":/images/play5.png"));
            }

        }
    }
    if(obj == ui->nextButton)               //����Ϊ��һ�װ�ť
    {
        if(e->type() == QEvent::Enter)
        {
            ui->nextButton->setIcon(QPixmap(":/images/next.png"));
        }
        if(e->type() == QEvent::Leave)
        {
            ui->nextButton->setIcon(QPixmap(":/images/next1.png"));
        }
    }
    if(obj == ui->lastButton)               //����Ϊ��һ�װ�ť
    {
        if(e->type() == QEvent::Enter)
        {
            ui->lastButton->setIcon(QPixmap(":/images/last.png"));
        }
        if(e->type() == QEvent::Leave)
        {
            ui->lastButton->setIcon(QPixmap(":/images/last1.png"));
        }
    }
    if(obj == ui->styleButton)              //������ť
    {
        if(e->type() == QEvent::Enter)
        {
            ui->styleButton->setIcon(QPixmap(":/images/picstyle2.png"));
        }
        if(e->type() == QEvent::Leave)
        {
            ui->styleButton->setIcon(QPixmap(":/images/picstyle.png"));
        }
    }

    return   QMainWindow::eventFilter(obj,e);

}

void MainWindow::on_lastButton_clicked()
{
    SkipBackward();
}
void MainWindow::on_styleButton_clicked()
{

    QColor color;
    color = QColorDialog::getColor(Qt::white, this);
    if(color.isValid())     //�ж��Ƿ���Ч��ɫ���ж��û��Ƿ�ֱ�ӹرնԻ���
    {
        QPalette p;
        p.setBrush(this->backgroundRole(),QBrush(color));
        this->setPalette(p);
        this->setAutoFillBackground(true);
        //qDebug()<<color.red();
        //qDebug()<<color.green();
        //qDebug()<<color.blue();
    }

}
void MainWindow::on_playButton_clicked()
{
    //�������״̬Ϊ��ͣ���򲥷�
    if(mediaObject->state() == Phonon::PlayingState)
    {

        mediaObject->pause();;

    }
    else
    {
        //�ж���Դ�Ƿ�Ϊ��
        if(sources.isEmpty()){
            QMessageBox::information(this, tr("Waring"), tr("�б�գ�"));
            return ;
        }

        mediaObject->play();
    }
}
void MainWindow::on_iconbutton_clicked()    //������Mͼ������Ӧ
{
    initRightMenu();
}
void MainWindow::on_playmode_clicked()
{
    QCursor cur = this->cursor();
    QMenu *menu = new QMenu(this);

    QAction *sc = new QAction(tr("����ѭ��"),this);    //����ѭ��
    QAction *sp = new QAction(tr("��������"),this);    //��������
    QAction *lp = new QAction(tr("˳�򲥷�"),this);    //˳�򲥷�
    QAction *lc = new QAction(tr("�б�ѭ��"),this);    //�б�ѭ��


    menu->addAction(sc);
    menu->addAction(sp);
    menu->addAction(lp);
    menu->addAction(lc);

    connect(sc,SIGNAL(triggered()),this,SLOT(playmode_sc_slot()));
    connect(sp,SIGNAL(triggered()),this,SLOT(playmode_sp_slot()));
    connect(lp,SIGNAL(triggered()),this,SLOT(playmode_lp_slot()));
    connect(lc,SIGNAL(triggered()),this,SLOT(playmode_lc_slot()));

    //���ò˵���ʽ
    menu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                        );

    menu->exec(cur.pos());  //����굱ǰλ����ʾ�˵�

}
void MainWindow::on_checkBox_list_stateChanged(int )  //��չ�Ի����Ƿ���ʾ�б�
{
    if(ui->checkBox_list->checkState()==Qt::Checked)
    {
        this->setFixedSize(this->width(),613);
        thelistopenflag = 1;
    }
    else
    {

        this->setFixedSize(this->width(),190);
        thelistopenflag = 0;
    }
}

void MainWindow::paintEvent(QPaintEvent *e)
{
    //��дpaintEvent������������߿�Բ��
    QPainter painter(this);
    painter.setPen(Qt::black);  //�߿�ɫ
    painter.drawRoundedRect(this->rect(), 5, 5); //Բ��5����
}
void MainWindow::initRightMenu()
{


    QCursor cur = this->cursor();
    rightmenu = new QMenu(this);
    QMenu *file = new QMenu(tr("�ļ�"),this);
    QMenu *play = new QMenu(tr("����"),this);
    QMenu *playmode_menu = new QMenu(tr("����ģʽ"),this);

    //QAction *load = new QAction(tr("��½"),this);

    QAction *showlrc = new QAction(tr("��ʾ������"),this);
    QAction *skin = new QAction(tr("����"),this);
    QMenu *help = new QMenu(tr("����"),this);
    QAction *quit = new QAction(tr("�˳�"),this);
    QAction *load_file = new QAction(tr("��ӱ��ظ���..."),this);
    QAction *load_files = new QAction(tr("��ӱ��ظ����ļ���..."),this);
    QAction *play2 = new QAction(tr("����/��ͣ"),this);
    QAction *last = new QAction(tr("��һ��"),this);
    QAction *next = new QAction(tr("��һ��"),this);
    QAction *addvol = new QAction(tr("��������"),this);
    QAction *dvol = new QAction(tr("��������"),this);
    QAction *novol = new QAction(tr("����"),this);

    QAction *author = new QAction(tr("����"),this);
    QAction *key    = new QAction(tr("��ݼ�"),this);

   // addvol->setShortcut(QKeySequence(Qt::Key_Up));
   // dvol->setShortcut(QKeySequence(Qt::Key_Down));
   // novol->setShortcut(QKeySequence("F6"));

    connect(addvol,SIGNAL(triggered()),this,SLOT(addvol_slot()));
    connect(dvol,SIGNAL(triggered()),this,SLOT(devvol_slot()));

    /* QActionGroup *skin_g = new QActionGroup(this);
    QActionGroup *playmode_g = new QActionGroup(this);
*/
    QAction *sc = new QAction(tr("����ѭ��"),this);    //����ѭ��
    QAction *sp = new QAction(tr("��������"),this);    //��������
    QAction *lp = new QAction(tr("˳�򲥷�"),this);    //˳�򲥷�
    QAction *lc = new QAction(tr("�б�ѭ��"),this);    //�б�ѭ��

    playmode_menu->addAction(sc);
    playmode_menu->addAction(sp);
    playmode_menu->addAction(lp);
    playmode_menu->addAction(lc);

    play->addAction(play2);
    play->addSeparator();
    play->addAction(last);
    play->addAction(next);
    play->addSeparator();
    play->addAction(addvol);
    play->addAction(dvol);
    play->addAction(novol);
    play->addSeparator();
    play->addMenu(playmode_menu);


    file->addAction(load_file);
    file->addAction(load_files);

    help->addAction(author);
    help->addAction(key);

    //rightmenu->addAction(load);
   // rightmenu->addSeparator();
    rightmenu->addMenu(file);
    rightmenu->addMenu(play);
    rightmenu->addSeparator();

    rightmenu->addAction(showlrc);
    rightmenu->addSeparator();
    rightmenu->addAction(skin);
    rightmenu->addSeparator();
    rightmenu->addMenu(help);
    rightmenu->addAction(quit);

    connect(load_file,SIGNAL(triggered()),this,SLOT(addfile()));
    connect(load_files,SIGNAL(triggered()),this,SLOT(addfile()));
    connect(skin,SIGNAL(triggered()),this,SLOT(on_styleButton_clicked()));
    connect(play2,SIGNAL(triggered()),this,SLOT(on_playButton_clicked()));
    connect(last,SIGNAL(triggered()),this,SLOT(on_lastButton_clicked()));
    connect(next,SIGNAL(triggered()),this,SLOT(on_nextButton_clicked()));
    connect(novol,SIGNAL(triggered()),this,SLOT(setSilent()));

    //connect(author,SIGNAL(triggered()),this,SLOT(showAuthor()));
    connect(key,SIGNAL(triggered()),this,SLOT(showKey()));

    help->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"
                        );
    rightmenu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                             "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                             "QMenu::item:selected:!enabled{background:transparent;}"
                             "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                             );
    play->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                        );
    file->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                        "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                        "QMenu::item:selected:!enabled{background:transparent;}"
                        "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                        );
    playmode_menu->setStyleSheet("QMenu{padding:5px;background:white;border:1px solid gray;}"
                                 "QMenu::item{padding:0px 40px 0px 30px;height:25px;}"
                                 "QMenu::item:selected:!enabled{background:transparent;}"
                                 "QMenu::item:selected:enabled{background:lightgray;color:white;}"

                                 );
    rightmenu->exec(cur.pos());

}
void MainWindow::contextMenuEvent(QContextMenuEvent *event) //�������Ҽ��˵�
{
    initRightMenu();
}

void MainWindow::addfile()      //����ļ�
{

    QStringList files = QFileDialog::getOpenFileNames(this, tr("�������ļ�"),
                        QCoreApplication::applicationDirPath(),tr("*.mp3;*.wma;*.wav"));


    if (files.isEmpty())
    {
        return;
    }
    int index = sources.size();
    foreach (QString string, files)
    {
        Phonon::MediaSource source(string);
        sources.append(source);

    }

    if(!sources.isEmpty())
    {
        //���ý��Դ�б�Ϊ�գ����¼���ĵ�һ��ý��Դ��Ϊ��ǰý��Դ
        metaInformationResolver->setCurrentSource(sources.at(index));
    }

}

//���µ�ǰʱ��
void MainWindow::updateTime(qint64 time)
{
    //ֱ�ӻ�ȡ����Ƶ�ļ�����ʱ����������λΪ����
    qint64 total_time_value = mediaObject->totalTime();

    //��3�������ֱ������ʱ���֣��룻60000����Ϊ1���ӣ����Է��ӵڶ����������ȳ�6000,��3��������ֱ�ӳ�1s
    QTime total_time(0, (total_time_value/60000)%60, (total_time_value/1000)%60);

    //����time��ʾ��ǰ����ʱ��
    QTime cur_time(0,(time/60000)%60, (time/1000)%60);

    //��ʽ
    QString total_time_str = total_time.toString("mm:ss");
    QString cur_time_str = cur_time.toString("mm:ss");

    //label����
    ui->songtotaltime->setText(total_time_str);
    ui->songcurtime->setText(cur_time_str);

    // ��ȡ����ʱ���Ӧ�ĸ��
    if(!lrc_map.isEmpty()) {
        // ��ȡ��ǰʱ���ڸ���е�ǰ������ʱ���
        qint64 previous = 0;
        qint64 later = 0;
        //keys()��������lrc_map�б�
        foreach (qint64 value, lrc_map.keys()) {
            if (time >= value) {
                previous = value;
            } else {
                later = value;
                break;
            }
        }

        // �ﵽ���һ��,��later����Ϊ������ʱ���ֵ
        if (later == 0)
            later = total_time_value;

        // ��ȡ��ǰʱ������Ӧ�ĸ������
        QString current_lrc = lrc_map.value(previous);

//        // û������ʱ
//        if(current_lrc.length() < 2)
//            current_lrc = tr("�������ֲ�����");

        // ������µ�һ�и�ʣ���ô���¿�ʼ��ʾ�������
        if(current_lrc != lrc->text()) {
            lrc->setText(current_lrc);
            //top_label->setText(current_lrc);
            qint64 interval_time = later - previous;
            lrc->start_lrc_mask(interval_time);
        }
    } else {  // ���û�и���ļ������ڶ�����ǩ����ʾ��������
        //top_label->setText(QFileInfo(mediaObject->
        //                            currentSource().fileName()).baseName());
    }
}
//����
void MainWindow::setSilent()
{
    if(audioOutput->volume()==0)
    {
        audioOutput->setVolume(0.8);
    }
    else
    {
        audioOutput->setVolume(0);
    }
}

//��ý�岥�ſ����ʱ���ᷢ��aboutToFinish()�źţ��Ӷ������òۺ���
void MainWindow::AboutToFinish()
{
    int index = sources.indexOf(mediaObject->currentSource())+1;
    switch(playmode)
    {
    case 1:         //��������
        mediaObject->stop();
        flag = 0;
        ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));
        playflag = 0;
        break;
    case 2:         //����ѭ��
        mediaObject->enqueue(sources.at(index-1));        //�����һ�׸赽�б�
        mediaObject->seek(mediaObject->totalTime());      //������ǰ������
        // mediaObject->stop();
        mediaObject->setCurrentSource(sources.at(index-1));
        mediaObject->play();
        //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

        lrc->stop_lrc_mask();
        resolve_lrc(sources.at(index-1).fileName());
        break;
    case 3:             //˳�򲥷�
        if(sources.size()>index)
        {
            mediaObject->enqueue(sources.at(index));        //�����һ�׸赽�б�
            mediaObject->seek(mediaObject->totalTime());      //������ǰ������
            // mediaObject->stop();
            mediaObject->setCurrentSource(sources.at(index));
            mediaObject->play();
            //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

            lrc->stop_lrc_mask();
            resolve_lrc(sources.at(index).fileName());
        }
        else
        {

            mediaObject->stop();        //����Ѿ��Ǵ���Ƶ�ļ������һ�׸��ˣ���ֱ��ֹͣ
            //��ص�����
            flag = 0;
            ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));
            playflag = 0;
        }
        break;
      case 4:
        if(sources.size()>index)
        {
            mediaObject->enqueue(sources.at(index));        //�����һ�׸赽�б�
            mediaObject->seek(mediaObject->totalTime());      //������ǰ������
            // mediaObject->stop();
            mediaObject->setCurrentSource(sources.at(index));
            mediaObject->play();
            //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

            lrc->stop_lrc_mask();
            resolve_lrc(sources.at(index).fileName());
        }
        else
        {

            mediaObject->enqueue(sources.at(0));        //�����һ�׸赽�б�
            mediaObject->seek(mediaObject->totalTime());      //������ǰ������
            // mediaObject->stop();
            mediaObject->setCurrentSource(sources.at(0));
            mediaObject->play();
            //ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

            lrc->stop_lrc_mask();
            resolve_lrc(sources.at(0).fileName());
        }


    }



}
//��һ��
void MainWindow::SkipBackward()
{
    lrc->stop_lrc_mask();
    int index = sources.indexOf(mediaObject->currentSource());
    if(index>0)
    {
        //mediaObject->stop();
        mediaObject->setCurrentSource(sources.at(index-1));
        mediaObject->play();
    }
}
//��һ��
void MainWindow::SkipForward()
{
    lrc->stop_lrc_mask();
    int index = sources.indexOf(mediaObject->currentSource());
    if(sources.size()>index+1)
    {
        //mediaObject->stop();
        mediaObject->setCurrentSource(sources.at(index+1));
        mediaObject->play();
    }
}

void MainWindow::on_nextButton_clicked()
{
    SkipForward();
}
//ý�岥��״̬�ı�ʱ��Ӧ
void MainWindow::StateChanged(Phonon::State new_state, Phonon::State old_state)
{
    QString num = tr("%1").arg(playsongNO+1);
    switch(new_state)
    {
    case Phonon::ErrorState:
        if(mediaObject->errorType() == Phonon::FatalError)
        {
            QMessageBox::warning(this, tr("��������"), mediaObject->errorString());//��ʾ�����
        }
        else
        {
            QMessageBox::warning(playlist, tr("����"), mediaObject->errorString());//��ʾ��ͨ����
        }

        break;
     case Phonon::PausedState:
          //���ð�ťͼƬ
          flag = 0;
          ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));

          playflag = 0;
          emit changethetraystate(Phonon::PausedState);
          ui->songnum->setText(tr("��Ŀ: ")+num);
          if (!lrc_map.isEmpty()) {
              lrc->stop_lrc_mask();
              lrc->setText(ui->songnamelabel->text());
          }
          break;
    case Phonon::StoppedState:
          flag = 0;
          ui->playButton->setIcon(QIcon(QPixmap(":/images/play4.png")));

          playflag = 0;
          ui->songnamelabel->setText("");
          ui->songtotaltime->setText("00:00");
          emit re_tray_songnamelabel();
          emit changethetraystate(Phonon::StoppedState);
          ui->songnum->setText("");
          lrc->stop_lrc_mask();
          lrc->setText(tr("M����"));

          break;


    case Phonon::PlayingState:
        //���ð�ťͼƬ
        flag = 1;
        ui->playButton->setIcon(QIcon(QPixmap(":/images/play1.png")));
        ui->songnamelabel->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName());

        playflag = 1;
        emit set_tray_songnamelabel(QFileInfo(mediaObject->currentSource().fileName()).baseName());
         emit changethetraystate(Phonon::PlayingState);

         ui->songnum->setText(tr("��Ŀ: ")+num);
         resolve_lrc(mediaObject->currentSource().fileName());
        firsttime = 1;
        break;

    case Phonon::BufferingState:
            break;
        default:
        ;
    }
}
void MainWindow::MetaStateChanged(Phonon::State new_state, Phonon::State old_state)
{
    // ����״̬�����ý��Դ�б��г�ȥ����ӵ�ý��Դ
    if(new_state == Phonon::ErrorState) {
        QMessageBox::warning(this, tr("���ļ�ʱ����"), metaInformationResolver->errorString());
        //takeLast()Ϊɾ�����һ�в����䷵��
        while (!sources.isEmpty() &&
               !(sources.takeLast() == metaInformationResolver->currentSource()))
        {};//ֻ�������һ��
        return;
    }
    // ����Ȳ�����ֹͣ״̬Ҳ��������ͣ״̬����ֱ�ӷ���
    if(new_state != Phonon::StoppedState && new_state != Phonon::PausedState)
        return;
    // ���ý��Դ���ʹ�����ֱ�ӷ���
    if(metaInformationResolver->currentSource().type() == Phonon::MediaSource::Invalid)
        return;

    QMap<QString, QString> meta_data = metaInformationResolver->metaData();//��ȡý��Դ�е�Դ����

    //��ȡ�ļ�������Ϣ
    QString title  = meta_data.value("TITLE");
    //���ý��Ԫ������û�б�����Ϣ����ȥ����Ƶ�ļ����ļ���Ϊ�ñ�����Ϣ
    if(title == "") {
    QString str = metaInformationResolver->currentSource().fileName();
    title = QFileInfo(str).baseName();
    }
    QTableWidgetItem *title_item = new QTableWidgetItem(title);
    title_item->setFlags(title_item->flags() ^ Qt::ItemIsEditable);

    //��ȡ��������Ϣ
    QTableWidgetItem *artist_item = new QTableWidgetItem(meta_data.value("ARTIST"));
    artist_item->setFlags(artist_item->flags() ^ Qt::ItemIsEditable);

    //��ȡ��ʱ����Ϣ
    qint64 total_time  = metaInformationResolver->totalTime();
    QTime time(0, (total_time/60000)%60, (total_time/10000)%60);
    QTableWidgetItem *time_item = new QTableWidgetItem(time.toString("mm:ss"));
    time_item->setFlags(time_item->flags() ^ Qt::ItemIsEditable);

    //���벥���б�
    int current_rows = playlist->rowCount();//�����б��е�����
    playlist->insertRow(current_rows);
    playlist->setItem(current_rows, 0, title_item);
    playlist->setItem(current_rows, 1, artist_item);
    playlist->setItem(current_rows, 2, time_item);

    //sourcesΪ�򿪵�������Ƶ�ļ��б�,playlistΪ���ֲ����б������
    int index = sources.indexOf(metaInformationResolver->currentSource())+1;
    if(sources.size() > index) //û�н�����
        metaInformationResolver->setCurrentSource(sources.at(index));
    else {
        //û�б�ѡ�е���
        if(playlist->selectedItems().isEmpty()) {
            // �������û�в��Ÿ��������õ�һ��ý��ԴΪý�����ĵ�ǰý��Դ
            //����Ϊ�������ڲ��Ÿ���ʱ����˲����б�Ȼ����������µ��б�
            if(mediaObject->state() != Phonon::PlayingState && mediaObject->state() != Phonon::PausedState)
                mediaObject->setCurrentSource(sources.at(0));
            else {
                //������ڲ��Ÿ�������ѡ�в����б�ĵ�һ����Ŀ,������ͼ��״̬
                playlist->selectRow(0);

            }
        }

    }
    QString total = tr("%1").arg(playlist->rowCount());

    ui->songtotal->setText("����Ŀ: "+total);

}
//Դ�����ı�
void MainWindow::SourceChanged(const Phonon::MediaSource &source)
{
    int index = sources.indexOf(source);
    //playsongNO��ǵ�ǰ������
    playsongNO = index;
    //playlist->selectRow(index);

    //���õ�ǰ���и������п���
    for(int i = 0;i<3;i++)
    {
        playlist->item(songNo,i)->setBackgroundColor(QColor(255,255,255));
        //playlist->item(songNo,i)->setTextColor(QColor(0,0,0));
    }
    for(int i = 0;i<3;i++)
    {
        playlist->item(index,i)->setBackgroundColor(QColor(225, 225, 225));
       // playlist->item(index,i)->setTextColor(QColor(0,255,255));

    }

    playlist->setRowHeight(songNo,30);
    playlist->setRowHeight(index,playlist->rowHeight(index)+30);

    songNo = index;
    /*if(playlist->rowCount()!=0)
    {
        QString total = tr("%1").arg(playlist->rowCount());

        ui->songtotal->setText("����Ŀ: "+total);
    }
    else ui->songtotal->setText("");
*/
}
//���Դ
void MainWindow::ClearSources()
{
    sources.clear();
    ui->songtotal->setText("");
}
//�б���˫��
void MainWindow::TableClicked(QTableWidgetItem* item)
{

    int row = item->row();
    bool was_palying = mediaObject->state() == Phonon::PlayingState;
    if(mediaObject->state() == Phonon::StoppedState)
    {
        mediaObject->setCurrentSource(sources.at(row));
        mediaObject->play();
        return;
    }
        mediaObject->stop();//ֹͣ��ǰ���ŵĸ���
        mediaObject->clearQueue();//������Ŷ���

        //������͵Ĳ����б��кű�ý��Դ���б��кŻ�����ֱ�ӷ���
        if(row >= sources.size())
            return;
        mediaObject->setCurrentSource(sources.at(row));
        if(was_palying)//���ѡ��ǰ�ڲ��Ÿ�������ôѡ�к�Ҳ�������Ÿ���
            mediaObject->play();
}

void MainWindow::traytochangvol(qreal vol)
{
    audioOutput->setVolume(vol);
}
void MainWindow::traytoclearlist_slot()
{
    while(playlist->rowCount())
        playlist->removeRow(0);
    ClearSources();
}

void MainWindow::volchange_slot(qreal qvol)
{
    emit vol_change_signal(qvol);
}
void MainWindow::addvol_slot()
{
    volumeSlider->setFocus();
    audioOutput->setVolume(audioOutput->volume()+0.1);
   // ui->songnamelabel->setText("UP");
}
void MainWindow::devvol_slot()
{
    volumeSlider->setFocus();   //�ÿؼ���ȡ����
    audioOutput->setVolume(audioOutput->volume()-0.1);
   // ui->songnamelabel->setText("DOWN");
}
void MainWindow::showThisWindow()
{
    this->showNormal();
    this->raise();
}
void MainWindow::SetLrcShown()
{
    if(lrc->isHidden())
        lrc->show();
    else
        lrc->hide();
}
// ����LRC��ʣ���stateChanged()������Phonon::PlayingState����aboutToFinish()�����е����˸ú���
void MainWindow::resolve_lrc(const QString &source_file_name)
{
    lrc_map.clear();
    if(source_file_name.isEmpty())
        return;
    QString file_name = source_file_name;
    QString lrc_file_name = file_name.remove(file_name.right(3)) + "lrc";//����Ƶ�ļ��ĺ�׺�ĳ�lrc��׺

    // �򿪸���ļ�
    QFile file(lrc_file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        lrc->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName()
                     + tr(" --- δ�ҵ�����ļ���"));
        return ;
    }
    // �����ַ�������
    QTextCodec::setCodecForCStrings(QTextCodec::codecForLocale());
    QString all_text = QString(file.readAll());
    file.close();
    // ����ʰ��зֽ�Ϊ����б�
    QStringList lines = all_text.split("\n");

    //�����ʱ���ǩ�ĸ�ʽ[00:05.54]
    //������ʽd{2}��ʾƥ��2������
    QRegExp rx("\\[\\d{2}:\\d{2}\\.\\d{2}\\]");
    foreach(QString oneline, lines) {
        QString temp = oneline;
        temp.replace(rx, "");//�ÿ��ַ����滻������ʽ����ƥ��ĵط�,�����ͻ���˸���ı�
        // Ȼ�����λ�ȡ��ǰ���е�����ʱ���ǩ�����ֱ������ı�����QMap��
        //indexIn()Ϊ���ص�һ��ƥ���λ�ã��������Ϊ-1�����ʾû��ƥ��ɹ�
        //���������pos����Ӧ�ö�Ӧ���Ǹ���ļ�
        int pos = rx.indexIn(oneline, 0);
        while (pos != -1) { //��ʾƥ��ɹ�
            QString cap = rx.cap(0);//���ص�0�����ʽƥ�������
            // ��ʱ���ǩת��Ϊʱ����ֵ���Ժ���Ϊ��λ
            QRegExp regexp;
            regexp.setPattern("\\d{2}(?=:)");
            regexp.indexIn(cap);
            int minute = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\.)");
            regexp.indexIn(cap);
            int second = regexp.cap(0).toInt();
            regexp.setPattern("\\d{2}(?=\\])");
            regexp.indexIn(cap);
            int millisecond = regexp.cap(0).toInt();
            qint64 totalTime = minute * 60000 + second * 1000 + millisecond * 10;
            // ���뵽lrc_map��
            lrc_map.insert(totalTime, temp);
            pos += rx.matchedLength();
            pos = rx.indexIn(oneline, pos);//ƥ��ȫ��
        }
    }
    // ���lrc_mapΪ��
    if (lrc_map.isEmpty()) {
        lrc->setText(QFileInfo(mediaObject->currentSource().fileName()).baseName()
                     + tr(" --- ����ļ����ݴ���"));
        return;
    }
}



void MainWindow::on_checkBox_lrc_stateChanged(int )
{
    this->SetLrcShown();
}

void MainWindow::playmode_sc_slot()
{
    playmode = 2;
    ui->playmode->setText("����ѭ��");
}
void MainWindow::playmode_sp_slot()
{
    playmode = 1;
    ui->playmode->setText("��������");
}
void MainWindow::playmode_lp_slot()
{
    playmode = 3;
    ui->playmode->setText("˳�򲥷�");
}
void MainWindow::playmode_lc_slot()
{
    playmode = 4;
    ui->playmode->setText("�б�ѭ��");
}

void MainWindow::on_toolButton_clicked()
{
    this->addfile();
}

void MainWindow::on_toolButton_2_clicked()
{
    this->on_playmode_clicked();
}
void MainWindow::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Up:
        addvol_slot();
        break;
    case Qt::Key_Down:
        devvol_slot();
        break;

    }
}
void MainWindow::showAuthor()
{
    QString s;
    s += "������\n";
    s += "������\n";
    QMessageBox::information(NULL, QString("����"),s);
}
void MainWindow::showKey()
{
    QString key ;
    key+="����/��ͣ   �ո�\n";
    key+="��һ��      ��\n";
    key+="��һ��      ��\n";
    key+="���ļ�    ctrl+o\n";
    key+="��������    ��\n";
    key+="��������    ��\n\n\n";
    key+="���ļ��϶���������ܼ����ļ���\n";
    QMessageBox::information(NULL, QString("��ݼ�"),key);
}
void MainWindow::writefile()
{
    QFile file ("QTDATA.dat");
    file.resize(0);                 //д��ǰ������ļ�����
    QString data;
    QTextStream stream(&file);

    //д�뱳����ɫ
    QPalette pal = this->palette();
    QBrush brush = pal.background();
    QColor col   = brush.color();

    data = QString::number(col.red(),10)+"\n";
    data += QString::number(col.green(),10)+"\n";
    data += QString::number(col.blue(),10)+"\n";

    //д�뵱ǰ�����Ͳ���ģʽ
    data += QString::number(audioOutput->volume(),'f',1)+"\n";
    data += QString::number(playmode,10)+"\n";
    data += QString::number(thelistopenflag,10)+"\n";
    //д�����������·��
    if(!sources.isEmpty())
    {
        for(int i = 0;i<sources.size();i++)
        {
            data += sources.at(i).url().toString().replace("file:///","")+"\n";
        }
    }
    file.open( QIODevice::WriteOnly| QIODevice::Text );

    stream<<data;
    file.close();
}
void MainWindow::readfile()
{
    QFile file ("QTDATA.dat");
    QString data;
    QTextStream stream(&file);
    int r,g,b,index = 0;
    double vol;

    if(!file.exists())
    {
        file.open( QIODevice::ReadWrite| QIODevice::Text );
    }
    else
    {
        file.open( QIODevice::ReadWrite| QIODevice::Text );
        while(!stream.atEnd())
        {
            data = stream.readLine();

            switch(index)
            {
            case 0:
                r = data.toInt();
                break;
            case 1:
                g = data.toInt();
                break;
            case 2:
                b = data.toInt();
                break;
            case 3:
                vol = data.toDouble();
                break;
            case 4:
                playmode = data.toInt();
                break;
            case 5:
                thelistopenflag = data.toInt();
                break;
            default:
                Phonon::MediaSource source(data);
                sources.append(source);
                if(!sources.isEmpty())
                {
                    metaInformationResolver->setCurrentSource(sources.at(0));
                }

            }
            index++;
        }

        file.close();


        QPalette p;
        p.setBrush(this->backgroundRole(),QBrush(QColor(r,g,b,255)));
        this->setPalette(p);
        this->setAutoFillBackground(true);

        setPlaymodebuttontext(playmode);
        setWindowHeight(thelistopenflag);
        audioOutput->setVolume(vol);
    }
}
void MainWindow::setPlaymodebuttontext(int mode)
{
    switch(mode)
    {
    case 1:
        ui->playmode->setText("��������");
        break;
    case 2:
        ui->playmode->setText("����ѭ��");
        break;
    case 3:
        ui->playmode->setText("˳�򲥷�");
        break;
    case 4:
        ui->playmode->setText("�б�ѭ��");
        break;
    }


}
void MainWindow::setWindowHeight(int flag)
{
    if(flag == 1)
    {

        ui->checkBox_list->setChecked(true);
    }
    else
    {

        ui->checkBox_list->setChecked(false);
    }
}

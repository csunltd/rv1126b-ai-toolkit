
#include "mainWidget.h"
#include "ui_mainWidget.h"
#include "system.h"
#include "system_opt.h"
//#include "network.h"

#include <QString>
#include <QList>
#include <QBitmap>
#include <QPixmap>
#include <QDebug>

using namespace std;

#define USE_PAGE_BY_CREATE

static int get_local_Ip(const char *interface, char *strIP)
{
    // ソケットを作成します
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }

    // ネットワークインターフェース名を指定します
    struct ifreq ifr;
    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    // IP アドレスを取得します
    if (ioctl(sockfd, SIOCGIFADDR, &ifr) < 0) {
        perror("ioctl");
        close(sockfd);
        return -1;
    }

    close(sockfd);

    inet_ntop(AF_INET, &(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr), strIP, INET_ADDRSTRLEN);

    return 0;
}

QString GBK2QString(char *pGbkCode)
{
    QByteArray array;
    QString strGbkCode = pGbkCode;
    for (int i=0; i<(strGbkCode.length() / 2); ++i)
    {
        uchar b = strGbkCode.mid(i*2, 2).toInt(nullptr, 16);
        array.append(b);
    }

    return QString::fromLocal8Bit(array);
}

mainWidget::mainWidget(QWidget *parent) :
    QWidget(parent),
    mpTextLab(NULL),
    ui(new Ui::Widget)
{
    //設定：ウィンドウ枠を非表示にします
    //setWindowFlag(Qt::FramelessWindowHint, true);

    //設定：背景を透明にします
    QPalette pal(this->palette());
    pal.setColor(QPalette::Background, Qt::transparent);
    setAutoFillBackground(true);
    setPalette(pal);

    ui->setupUi(this);
    init();
}

mainWidget::~mainWidget()
{

    delete ui;
}

void mainWidget::init()
{
    layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);//ここではすべてのパラメータページの余白を設定しています

    //left
    ui->mpDateLab->setStyleSheet("QLabel { font: 24px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpWeekLab->setStyleSheet("QLabel { font: 36px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpProductLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");
    ui->mpSNLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");

    //right
    ui->mpTimeLab->setStyleSheet("QLabel { font: 72px 'Ubuntu'; color: rgb(245, 245, 245); background-color: transparent; }");
    ui->mpIPLab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");
//    ui->mpWIFILab->setStyleSheet("QLabel { font: 20px 'Ubuntu'; color: rgb(153, 153, 153); background-color: transparent; }");

    getSysDateTime();
    updateSysDateTime();
    updateSysIpParam();
    updateCPUSerial();

    mpTextLab = new QLabel;
    mpTextLab->setStyleSheet("QLabel { font: 75 16pt 'Ubuntu'; color: rgb(162, 195, 56); background-color: transparent; }");
    //QFont font("Microsoft YaHei", 12, 87);
    //mpTextLab->setFont(font);
    mpTextLab->setAlignment(Qt::AlignCenter);
    mpTextLab->setText(QString("easyeai_api へようこそ"));
    layout->addWidget(mpTextLab);
    ui->mpMainPage->setLayout(layout);
}

void mainWidget::getSysDateTime()
{
    uint32_t curDate = 0, curTime = 0;
    get_system_date_time(&curDate, &curTime);

    m_dateYear   = curDate/10000;
    m_dateMonth  = curDate%10000/100;
    m_dateDay    = curDate%100;
    m_dateWeek   = calc_week_day(m_dateYear, m_dateMonth, m_dateDay);
    m_timeHour   = curTime/10000;
    m_timeMin    = curTime%10000/100;
    m_timeSecond = curTime%100;

}

void mainWidget::updateSysDateTime()
{
    QString strDate;
    strDate.clear();
    strDate.append(QString::number(m_dateYear));
    strDate.append(tr("年"));
    strDate.append(QString::number(m_dateMonth).asprintf("%02d",m_dateMonth));
    strDate.append(tr("月"));
    strDate.append(QString::number(m_dateDay).asprintf("%02d",m_dateDay));
    strDate.append(tr("日"));

    QString strWeek;
    strWeek.clear();
    switch (m_dateWeek) {
    case 1:
        strWeek.append("月曜日");
        break;
    case 2:
        strWeek.append("火曜日");
        break;
    case 3:
        strWeek.append("水曜日");
        break;
    case 4:
        strWeek.append("木曜日");
        break;
    case 5:
        strWeek.append("金曜日");
        break;
    case 6:
        strWeek.append("土曜日");
        break;
    case 7:
        strWeek.append("日曜日");
        break;
    default:
        break;
    }

    QString strTime;
    strTime.clear();
    strTime.append(QString::number(m_timeHour).asprintf("%02d",m_timeHour));
    strTime.append(":");
    strTime.append(QString::number(m_timeMin).asprintf("%02d",m_timeMin));
//    strTime.append(":");
//    strTime.append(QString::number(m_timeSecond).sprintf("%02d",m_timeSecond));

//============================================================================================
    ui->mpDateLab->setText(strDate);
    ui->mpWeekLab->setText(strWeek);
    ui->mpTimeLab->setText(strTime);
}

void mainWidget::updateSysIpParam()
{
    printf("mainWidget::updateSysIpParam() ... \n");
    QString ipAddress;
    ipAddress.clear();

    char ip[INET_ADDRSTRLEN]={0};
    msleep(200);    //IP を読み込む前に少し待機し、IP 設定が完了する前にすぐ読み込まれることを避けます
    if(0 != get_local_Ip("eth0", ip))
        return ;

    ipAddress.append(ip);
    ui->mpIPLab->setText(ipAddress);
}

void mainWidget::updateCPUSerial()
{
    QString cpuId;
    cpuId.clear();
    cpuId.append("SN: ");

    char id[32]={0};
    if(0 == exec_cmd_by_popen("cat /proc/cpuinfo | grep Serial | awk '{print $3}'", id)){
        id[strlen(id)-1] = 0;
        cpuId.append(id);
    }

    ui->mpSNLab->setText(cpuId);
}

void mainWidget::on_mpStandbyBtn_clicked()
{
    if(mpTextLab){
        if(mpTextLab->isHidden())
            mpTextLab->show();
        else
            mpTextLab->hide();
    }
}

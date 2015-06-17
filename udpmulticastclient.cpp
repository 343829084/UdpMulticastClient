#include "udpmulticastclient.h"

UdpMulticastClient::UdpMulticastClient(QObject *parent) :
    QObject(parent)
{
    system("route add -net 224.0.0.0 netmask 224.0.0.0 eth0");

    //tcp通信
    receive = new ReceiveFileServer(this);
    connect(receive, SIGNAL(message(QString)), this, SLOT(updateReceiveStatus(QString)));
    receive->startServer(5902);

    //组播通信
    ProcessDatagramTimer = new QTimer(this);
    ProcessDatagramTimer->setInterval(400);
    connect(ProcessDatagramTimer,SIGNAL(timeout()),this,SLOT(slotProcessPendingDatagrams()));
    ProcessDatagramTimer->start();

    RecvSocketFd = socket(AF_INET,SOCK_DGRAM,0); /*建立套接字*/
    if(RecvSocketFd == -1){
        perror("socket()");
        return;
    }

    SendSocketFd = socket(AF_INET,SOCK_DGRAM,0);
    if(SendSocketFd == -1){
        perror("socket()");
        return;
    }

    //用来从组播组接收消息
    memset(&RecvMulticastAddr,0,sizeof(struct sockaddr_in));
    RecvMulticastAddr.sin_family = AF_INET;
    RecvMulticastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    RecvMulticastAddr.sin_port = htons(MCAST_RECV_PORT);

    //用来给组播组发送消息
    memset(&SendMulticastAddr,0,sizeof(struct sockaddr_in));
    SendMulticastAddr.sin_family = AF_INET;
    SendMulticastAddr.sin_addr.s_addr =
            inet_addr(MCAST_SEND_ADDR);
    SendMulticastAddr.sin_port = htons(MCAST_SEND_PORT);

    /* 绑定自己的端口和IP信息到socket上 */
    if(bind(SendSocketFd,(struct sockaddr*)&SendMulticastAddr,sizeof(struct sockaddr_in)) < 0){
        perror("bind()");
        return;
    }

    /* 绑定自己的端口和IP信息到socket上 */
    if(bind(RecvSocketFd,(struct sockaddr*)&RecvMulticastAddr,sizeof(struct sockaddr_in)) < 0){
        perror("bind()");
        return;
    }

    if(fcntl(RecvSocketFd,F_SETFL,O_NONBLOCK) < 0){
        perror("fcntl()");
        return;
    }

    /*禁止回环许可,即本机不能收到自己发送给组播地址的消息*/
    int loop = 0;
    if(setsockopt(SendSocketFd,IPPROTO_IP,IP_MULTICAST_LOOP,&loop,
                  sizeof(loop)) < 0){
        perror("setsockopt():IP_MULTICAST_LOOP");
        return;
    }

    /*组播地址*/
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_RECV_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    /*把本机加入组播地址,即本机网卡作为组播成员,只有加入组才能收到组播消息*/
    if(setsockopt(RecvSocketFd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0){
        perror("setsockopt():IP_ADD_MEMBERSHIP");
        return;
    }

    iSocketLen = sizeof(struct sockaddr_in);
}

UdpMulticastClient::~UdpMulticastClient()
{
    /*退出组播组*/
    setsockopt(RecvSocketFd,IPPROTO_IP,IP_DROP_MEMBERSHIP,&mreq,sizeof(mreq));
    ::close(RecvSocketFd);
}

void UdpMulticastClient::slotProcessPendingDatagrams()
{
    memset(ReceiveBuffer,0,BUFF_SIZE);
    if(recvfrom(RecvSocketFd,ReceiveBuffer,sizeof(ReceiveBuffer),0,(struct sockaddr*)&RecvMulticastAddr,&iSocketLen) < 0){
        //        perror("recvfrom()");
        return;
    }
    qDebug() << ReceiveBuffer;

    char *ptrMessageHeader = strtok(ReceiveBuffer,"###");
    char *ptrMessageType = strtok(NULL,"###");
    if((strcmp(ptrMessageHeader,"Server") == 0) &&
            (strcmp(ptrMessageType,"SearchDevice") == 0)){
        QString ClientNetWorkInfo = CommonSetting::ReadFile("/etc/eth0-setting");
        QString ClientMac = CommonSetting::ReadFile("/sys/class/net/eth0/address");
        QString ClientCongfigureInfo = CommonSetting::ReadFile("/bin/config.ini");
        QString VersionInfo = CommonSetting::ReadFile("/bin/VersionInfo.ini");
        QString TotalMessage = QString("Client###SearchDevice###") + ClientNetWorkInfo + QString("MAC=") + ClientMac + QString("###") + ClientCongfigureInfo + QString("###") + VersionInfo;
        if(sendto(SendSocketFd,TotalMessage.toAscii().data(),TotalMessage.count(),0,(struct sockaddr*)&SendMulticastAddr,
                  sizeof(struct sockaddr_in)) < 0){
            perror("sendto()");
            return;
        }
    }else if((strcmp(ptrMessageHeader,"Server") == 0) &&
             (strcmp(ptrMessageType,"Configure") == 0)){
        char *ptrConfigureDeviceIP = strtok(NULL,"###");
        if(strcmp(GetLocalHostIP(),ptrConfigureDeviceIP) == 0){
#ifdef ShuaKaJiConfigBranch
            char *VaildConfigureInfo = strtok(NULL,"###");
            char *ServerIP = strtok(VaildConfigureInfo,",");
            char *ServerListenPort = strtok(NULL,",");
            char *HeartIntervalTime = strtok(NULL,",");
            char *RelayOnTime = strtok(NULL,",");
            char *MaxTime = strtok(NULL,",");
            char *DeviceID = strtok(NULL,",");
            char *DevelopBoardIP = strtok(NULL,",");
            char *DevelopBoardMask = strtok(NULL,",");
            char *DevelopBoardGateWay = strtok(NULL,",");
            char *DevelopBoardDNS = strtok(NULL,",");
            char *SmartUSBNumber = strtok(NULL,",");

            CommonSetting::WriteSettings("/bin/config.ini","time/RelayOnTime",RelayOnTime);
            CommonSetting::WriteSettings("/bin/config.ini","time/MaxTime",MaxTime);
            CommonSetting::WriteSettings("/bin/config.ini","SmartUSB/Num",SmartUSBNumber);
#endif

#ifdef JiaYouZhanConfigBranch
            char *VaildConfigureInfo = strtok(NULL,"###");
            char *ServerIP = strtok(VaildConfigureInfo,",");
            char *ServerListenPort = strtok(NULL,",");
            char *HeartIntervalTime = strtok(NULL,",");
            char *SwipCardIntervalTime = strtok(NULL,",");
            char *DeviceID = strtok(NULL,",");
            char *DevelopBoardIP = strtok(NULL,",");
            char *DevelopBoardMask = strtok(NULL,",");
            char *DevelopBoardGateWay = strtok(NULL,",");
            char *DevelopBoardDNS = strtok(NULL,",");

            CommonSetting::WriteSettings("/bin/config.ini","time/SwipCardIntervalTime",SwipCardIntervalTime);
#endif

#if  defined(ShuaKaJiConfigBranch) || defined(JiaYouZhanConfigBranch)

            CommonSetting::WriteSettings("/bin/config.ini","ServerNetwork/IP",ServerIP);
            CommonSetting::WriteSettings("/bin/config.ini","ServerNetwork/PORT",ServerListenPort);
            CommonSetting::WriteSettings("/bin/config.ini","time/HeartIntervalTime",HeartIntervalTime);
            CommonSetting::WriteSettings("/bin/config.ini","DeviceID/ID",DeviceID);

            QStringList DevelopConfigureInfo;
            DevelopConfigureInfo << tr("IP=%1\n").arg(DevelopBoardIP)
<< tr("Mask=%1\n").arg(DevelopBoardMask) << tr("Gateway=%1\n").arg(DevelopBoardGateWay) << tr("DNS=%1\n").arg(DevelopBoardDNS) << tr("MAC=%1\n").arg("08:90:90:90:90:90");
            QString ConfigureInfo = DevelopConfigureInfo.join("");

            QString fileName("/etc/eth0-setting");
            CommonSetting::WriteFile(fileName,ConfigureInfo);

            sendto(SendSocketFd,"Client###Configure###OK",sizeof("Client###Configure###OK"),0,(struct sockaddr*)&SendMulticastAddr,sizeof(struct sockaddr_in));
            CommonSetting::Sleep(1000);
            system("reboot");
#endif
        }
    }
}

char *UdpMulticastClient::GetLocalHostIP()//获取本机IP地址
{
    int Socketfd;
    struct sockaddr_in sin;
    struct ifreq ifr;

    Socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (Socketfd < 0){
        perror("socket");
        return NULL;
    }

    strncpy(ifr.ifr_name,"eth0",IFNAMSIZ);
    ifr.ifr_name[IFNAMSIZ - 1] = 0;

    if (ioctl(Socketfd,SIOCGIFADDR,&ifr) < 0){
        perror("ioctl");
        return  NULL;
    }

    memcpy(&sin,&ifr.ifr_addr,sizeof(sin));
    return inet_ntoa(sin.sin_addr);
}


void UdpMulticastClient::updateReceiveStatus(QString msg)
{
    qDebug() << "接收文件服务端:" << msg;
}

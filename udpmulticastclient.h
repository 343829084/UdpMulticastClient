#ifndef UDPMULTICASTCLIENT_H
#define UDPMULTICASTCLIENT_H

#include "CommonSetting.h"
#include "receivefileserver.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#define MCAST_SEND_PORT                  8888
#define MCAST_SEND_ADDR                  "224.0.0.88"
#define MCAST_RECV_PORT                  8889
#define MCAST_RECV_ADDR                  "224.0.0.89"
#define MCAST_INTERVAL                   1
#define BUFF_SIZE                        1025
#define FILE_NAME_LENGTH                 40

#define JiaYouZhanConfigWiegBranch
//#define JiaYouZhanConfigCertificateBranch
//#define ShuaKaJiConfigBranch

class UdpMulticastClient : public QObject
{
    Q_OBJECT

public:
    explicit UdpMulticastClient(QObject *parent = 0);
    ~UdpMulticastClient();
    char *GetLocalHostIP();
    void GetLocalHostMac(char *mac_addr);
    char *GetLocalHostMask();
//    char *GetLocalHostGateWay();

private slots:
    void slotProcessPendingDatagrams();
    void updateReceiveStatus(QString msg);

private:
    struct sockaddr_in RecvMulticastAddr;//从组播接受消息
    struct sockaddr_in SendMulticastAddr;//用来给组播发送消息
    struct ip_mreq mreq;
    socklen_t iSocketLen;
    int RecvSocketFd;
    int SendSocketFd;
    char ReceiveBuffer[BUFF_SIZE];
    char SendBuffer[BUFF_SIZE];

    QTimer *ProcessDatagramTimer;

    int receiveBytes;
    qint64 receiveBlockNumber;
    qint64 receiveMaxBytes;

    ReceiveFileServer *receive;
};

#endif // UDPMULTICASTCLIENT_H

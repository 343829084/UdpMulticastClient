#include "udpmulticastclient.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CommonSetting::SetUTF8Code();

    UdpMulticastClient w;

    system("rm -rf QBaseWidget.tgz UdpMulticastClient iconhelper.tgz");
    system("rm -rf /usr/local/ntp/ /usr/local/smplayer/ /usr/local/iconhelper/ /usr/local/qtopia-titles-*");
    system("rm -rf /opt/Qtopia4.4.3 /opt/kde /root/Documents");

    return a.exec();
}

#include <QDebug>
#include "mirall/smbclient.h"

int main(int argc, char **argv)
{
    Mirall::SmbClient client;
    qDebug() << client.scan();
    return 0;
}

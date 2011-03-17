
#include <errno.h>
#include <libsmbclient.h>
#include <QDebug>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include "mirall/smbclient.h"

static void
no_auth_data_fn(const char * pServer,
                const char * pShare,
                char * pWorkgroup,
                int maxLenWorkgrou,
                char * pUsername,
                int maxLenUsername,
                char * pPassword,
                int maxLenPassword)
{
    return;
}

static void
get_auth_data_with_context_fn(SMBCCTX * context,
                              const char * pServer,
                              const char * pShare,
                              char * pWorkgroup,
                              int maxLenWorkgroup,
                              char * pUsername,
                              int maxLenUsername,
                              char * pPassword,
                              int maxLenPassword)
{
    return;
}

namespace Mirall {

class SmbScanThread::Private
{
public:
    QMutex *scanMutex;
};

SmbScanThread::SmbScanThread(QObject *parent, QMutex *scanMutex)
    : QThread(parent), d(new Private())
{
    d->scanMutex = scanMutex;
}

SmbScanThread::~SmbScanThread()
{
    qDebug() << "SmbScanThread::~SmbScanThread";

    delete d;
}

void SmbScanThread::run()
{
    qDebug() << "before mutex lock";

    QMutexLocker(d->scanMutex);

    qDebug() << "danger zone";


    int dir;
    struct stat stat;
    struct smbc_dirent *dirent;

    if ((dir = smbc_opendir("smb://LINKSTATION")) < 0)
    {
        qDebug() << "FAILED!";

        //qDebug("Could not open directory [%s] (%d:%s)\n",
        //     "smb://", errno, strerror(errno));
    }

    while ((dirent = smbc_readdir(dir)) != NULL)
    {
        qDebug() << "result" << dirent->name;
        switch(dirent->smbc_type)
        {
            case SMBC_FILE_SHARE:
                emit scanResult(QString::fromUtf8(dirent->name));
                break;
        default:
            break;
        }
    }
    smbc_closedir(dir);

    qDebug() << "finish danger zone";

}


class SmbClient::Private
{
public:
    SMBCCTX * context;
    QMutex scanMutex;
};

SmbClient::SmbClient(QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    //smbc_init();
    /* Allocate a new context */
    d->context = smbc_new_context();
    if (!d->context) {
        qDebug() << "Could not allocate new smbc context";
    }
    smbc_option_set(d->context, "debug_stderr", (void *) 1);
    if (!smbc_init_context(d->context)) {
        qDebug() << "Failed to initialize context";
    }

    //d->context->callbacks.auth_fn = no_auth_data_fn;

    smbc_option_set(d->context,
                    "auth_function",
                    (void *) get_auth_data_with_context_fn);

    smbc_set_context(d->context);
}


void SmbClient::scan()
{
    SmbScanThread *thread = new SmbScanThread(this, &d->scanMutex);
    QObject::connect(thread, SIGNAL(scanResult(const QString &)), this, SIGNAL(scanResult(const QString &)));
    thread->start();
}

SmbClient::~SmbClient()
{
    qDebug() << "SmbClient::~SmbClient";

    smbc_free_context(d->context, 0);
    delete d;
}

} //ns mirall

#include "smbclient.moc"

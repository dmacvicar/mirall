
#ifndef MIRALL_SMBCLIENT_H
#define MIRALL_SMBCLIENT_H

#include <QMutex>
#include <QObject>
#include <QStringList>
#include <QThread>

namespace Mirall {

class SmbScanThread : public QThread
{
Q_OBJECT
public:
    SmbScanThread(QObject *parent, QMutex *scanMutex);
    ~SmbScanThread();
    void run();
signals:
    void scanResult(const QString &name);
private:
    class Private;
    Private *d;
};

class SmbClient : public QObject
{
Q_OBJECT
public:
  SmbClient(QObject *parent = 0L);
  ~SmbClient();

  /**
   * scans
   */
  void scan();

signals:

  void scanResult(const QString &name);

private:
  class Private;
  Private *d;
};

} // ns

#endif

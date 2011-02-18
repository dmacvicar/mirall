#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class QAction;
class QSystemTrayIcon;

namespace Mirall {

class Folder;
class SmbClient;

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int argc, char **argv);
    ~Application();
signals:

protected slots:

    void slotAddFolder();

    void slotShare(const QString &share);

protected:

    void setupActions();
    void setupSystemTray();
    void setupContextMenu();

private:
    Folder *_folder;
    QSystemTrayIcon *_tray;
    QAction *_actionQuit;
    QAction *_actionAddFolder;
    SmbClient *_smbClient;
};

} // namespace Mirall

#endif // APPLICATION_H

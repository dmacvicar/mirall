#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class QAction;
class QSystemTrayIcon;
class QNetworkConfigurationManager;

namespace Mirall {

class Folder;

class Application : public QApplication
{
    Q_OBJECT
public:
    explicit Application(int argc, char **argv);
    ~Application();
signals:

protected slots:

    void slotAddFolder();

protected:

    void setupActions();
    void setupSystemTray();
    void setupContextMenu();

    void setupKnownFolders();

private:
    Folder *_folder;
    QSystemTrayIcon *_tray;
    QAction *_actionQuit;
    QAction *_actionAddFolder;
    QNetworkConfigurationManager *_networkMgr;

};

} // namespace Mirall

#endif // APPLICATION_H

#include "mainwindow.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>


class PigmentPicker : public QApplication
{
public:
    MainWindow w;
    PigmentPicker(int argc, char *argv[]) : QApplication(argc, argv)
    {
        // Create the system tray icon
        QSystemTrayIcon* trayIcon = new QSystemTrayIcon(QIcon(":/new/prefix1/ColPicker.png"), this);
        //":/icons/pigment.png"
        trayIcon->setToolTip("Shade Saver");
        trayIcon->show();

        // Create the menu
        QMenu* trayMenu = new QMenu();
        // Connect to main windows show/hide
        trayMenu->addAction("Toggle Open/Taskbar", &w, SLOT(show_hide()));
        // Connect to new colour in main window
        trayMenu->addAction("New Colour", &w, SLOT(on_actionAdd_New_Colour_triggered()));

        trayMenu->addAction("Exit", this, SLOT(quit()));

        trayIcon->setContextMenu(trayMenu);

    }
};

int main(int argc, char *argv[])
{
    PigmentPicker a(argc, argv);
    //MainWindow w;
    a.w.show();
    return a.exec();
}

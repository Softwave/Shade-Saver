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
        trayIcon->setToolTip("Shade Saver: Double click to show/hide");
        trayIcon->show();

        // Create the menu
        QMenu* trayMenu = new QMenu();

        // Connect to new colour in main window
        trayMenu->addAction("New Color", &w, SLOT(on_actionAdd_New_Color_triggered()));

        trayMenu->addAction("Exit", this, SLOT(quit()));

        trayIcon->setContextMenu(trayMenu);

        // Clicking on the tray icon will show/hide the main window
        connect(trayIcon, &QSystemTrayIcon::activated, [this](auto reason)
        {
            if (reason == QSystemTrayIcon::DoubleClick)
            {
                w.show_hide();
            }
        });
    }
};

int main(int argc, char *argv[])
{
    PigmentPicker a(argc, argv);


    a.w.show();
    return a.exec();
}

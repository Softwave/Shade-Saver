#include "mainwindow.h"

#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>




int main(int argc, char *argv[])
{
    //PigmentPicker a(argc, argv);
    QApplication app(argc, argv);
    MainWindow w;

    // Create the system tray icon
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(QIcon(":/new/prefix1/ColPicker.png"));
    //":/icons/pigment.png"
    trayIcon->setToolTip("Shade Saver: Double click to show/hide");
    trayIcon->show();

    // Create the menu
    QMenu* trayMenu = new QMenu();

    // Connect to new colour in main window
    trayMenu->addAction("New Color", &w, SLOT(on_actionAdd_New_Color_triggered()));

    trayMenu->addAction("Show/Hide", &w, SLOT(show_hide()));

    trayMenu->addAction("Exit", &app, SLOT(quit()));

    trayIcon->setContextMenu(trayMenu);

    // Clicking on the tray icon will show/hide the main window
    app.connect(trayIcon, &QSystemTrayIcon::activated, [&w](auto reason)
                {
                    if (reason == QSystemTrayIcon::Trigger)
                    {
                        //w.show_hide();
                        w.show_hide();
                    }

                    if (reason == QSystemTrayIcon::MiddleClick)
                    {
                        w.show_hide();
                    }

                });


    w.show();
    return app.exec();
}

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColorDialog>
#include <QString>
#include <QMap>
#include <QListWidget>
#include <QListWidgetItem>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QSystemTrayIcon>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void show_hide();

private slots:
    void on_actionAdd_New_Color_triggered();



    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_actionLoad_Colors_From_File_triggered();

    void on_actionSave_Colors_to_File_triggered();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void updateInterface();

    void refreshOurColorsFromUI();

    void on_actionAbout_Pigment_triggered();



    void on_MainWindow_customContextMenuRequested(const QPoint &pos);

    void ProvideContextMenu(const QPoint &);


    void on_actionNew_Empty_Color_List_triggered();

private:
    Ui::MainWindow *ui;
    QMap<int, QString> ourColors;
    int currentRow;
    QString currentName;
    bool eventFilter(QObject *watched, QEvent *event);
    void reorderList();
    bool closing;
protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H

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

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionAdd_New_Colour_triggered();



    void on_listWidget_itemChanged(QListWidgetItem *item);

    void on_actionLoad_Colours_From_File_triggered();

    void on_actionSave_Colours_to_File_triggered();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void updateInterface();

    void refreshOurColorsFromUI();

    void on_actionAbout_Pigment_triggered();

    void show_hide();

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
};
#endif // MAINWINDOW_H

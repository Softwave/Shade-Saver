#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>

QColor getInvertedColor(QColor color)
{
    // Get the luminance of the bg color
    double luminance = 0.2126 * pow(color.red()/255.0, 2.2) +
                       0.7152 * pow(color.green()/255.0, 2.2) +
                       0.0722 * pow(color.blue()/255.0, 2.2);

    // Set black or white based on the luminance of the color
    if (luminance > 0.5)
        return QColor(Qt::black);
    else
        return QColor(Qt::white);
}

//

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    closing = false;
    ui->setupUi(this);

    // Setup the list widget
    ui->listWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listWidget->setDragEnabled(true);
    ui->listWidget->viewport()->setAcceptDrops(true);
    ui->listWidget->setDropIndicatorShown(true);
    ui->listWidget->setDragDropMode(QAbstractItemView::InternalMove);

    // Context menu
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(ProvideContextMenu(const QPoint &)));

    //Add event filter
    ui->listWidget->installEventFilter(this);

    // Set the listwidget to nofocus
    ui->listWidget->setFocusPolicy(Qt::NoFocus);

}

void MainWindow::show_hide()
{
    if (this->isVisible())
    {
        this->hide();
    }
    else
    {
        this->show();
    }
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAdd_New_Color_triggered()
{
    // Show if hidden
    if (!this->isVisible())
    {
        this->show();
    }

    QColor Color = QColorDialog::getColor(Qt::white, this, "Select Color");
    QString ColorName = Color.name();

    QListWidgetItem* newItem = new QListWidgetItem(ColorName);
    newItem->setBackgroundColor(Color);
    newItem->setTextColor(getInvertedColor(Color));

    int insertRow = ui->listWidget->currentRow() + 1;
    if (insertRow > 0 && insertRow <= ui->listWidget->count()) {
        // Insert the new item below the current selection
        ui->listWidget->insertItem(insertRow, newItem);
    } else {
        // Add the new item to the end of the list
        ui->listWidget->addItem(newItem);
        insertRow = ui->listWidget->count() - 1;
    }

    newItem->setFlags(newItem->flags().setFlag(Qt::ItemIsEditable, false));
    ourColors.insert(insertRow, ColorName);

    // Copy the Color name to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ColorName);

    // Select the new item
    //ui->listWidget->setCurrentRow(insertRow);
    ui->listWidget->clearFocus();
    ui->listWidget->setFocusPolicy(Qt::NoFocus);

}

// Clear the selection when we click on the main window
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        ui->listWidget->clearSelection();
        // Stop underlining the selected item
        ui->listWidget->clearFocus();
    }
}



void MainWindow::refreshOurColorsFromUI()
{
    ourColors.clear();
    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* curItem = ui->listWidget->item(i);
        ourColors.insert(i, curItem->text());
    }
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->listWidget)
    {
        if (event->type() == QEvent::ChildRemoved)
        {
            reorderList();
            return true;
        }
        else
        {
            return false;
        }
    }
}

void MainWindow::reorderList()
{
    ourColors.clear();

    for (int i = 0; i < ui->listWidget->count(); ++i)
    {
        QListWidgetItem* curItem = ui->listWidget->item(i);
        ourColors.insert(i, curItem->text());
    }
    ui->listWidget->clearSelection();
    ui->listWidget->clearFocus();
}

void MainWindow::on_listWidget_itemChanged(QListWidgetItem *item)
{
    item->setBackgroundColor(item->text());
}


void MainWindow::on_actionLoad_Colors_From_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Colors File"), "",
                                                    tr("Colors File (*.clrs);;All Files (*)"));


    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_5);


    ourColors.clear();   // clear our Colors
    ui->listWidget->clear();

    in >> ourColors;


    QMap<int, QString>::iterator i;
    for (i = ourColors.begin(); i != ourColors.end(); i++)
    {
        currentRow = i.key();
        currentName = i.value();
        updateInterface();
    }

    // Stop underlining the text even though we don't have a selection
    ui->listWidget->clearFocus();
    ui->listWidget->clearSelection();
    ui->listWidget->repaint();
}


void MainWindow::updateInterface()
{
    ui->listWidget->addItem(currentName);
    QListWidgetItem* curItem = ui->listWidget->item(ui->listWidget->count()-1);
    curItem->setText(currentName);
    curItem->setBackgroundColor(curItem->text());
    curItem->setTextColor(getInvertedColor(QColor(curItem->text())));
    curItem->setFlags(curItem->flags()
                          .setFlag(Qt::ItemIsEditable, false));
}


void MainWindow::on_actionSave_Colors_to_File_triggered()
{
    refreshOurColorsFromUI();
    qDebug() << "Number of Colors being saved:" << ourColors.size();

    QString filter = "Colors File (*.clrs)";
    QFileDialog dialog(this, tr("Save Colors File"), "", filter);
    dialog.setDefaultSuffix(".clrs");
    QString fileName = dialog.getSaveFileName(this, tr("Save Colors File"), "", filter, &filter);



    if (fileName.isEmpty())
        return;
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly))
        {
            QMessageBox::information(this, tr("?Unable to open file"),
                                     file.errorString());
            return;
        }
        QDataStream out(&file);
        out.setVersion(QDataStream::Qt_5_0);
        out << ourColors;
    }
}


void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QColor originalColor = item->backgroundColor(); // Store the original color
    QColor newColor = QColorDialog::getColor(originalColor, this, "Select Color");

    // Check if the newColor is valid before applying it
    if (newColor.isValid()) {
        item->setBackgroundColor(newColor);
        item->setTextColor(getInvertedColor(newColor));
        item->setText(newColor.name());

        // Copy the newColor name to the clipboard
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(newColor.name());

        // Clear focus
        ui->listWidget->clearFocus();
        ui->listWidget->clearSelection();
    }
    // If newColor is not valid do not change anything
}



void MainWindow::on_actionAbout_Pigment_triggered()
{
    QMessageBox aboutBox;
    aboutBox.setWindowTitle("About Shade Saver");
    aboutBox.setText("Shade Saver; pick and keep track of colors!");
    aboutBox.setInformativeText("Softwave, 2025\n GPL v3 see license for more information\nwhttps://s0ftwave.net/");
    aboutBox.exec();
}


void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
    QPoint item = ui->listWidget->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction("Delete");
    QAction* rightClickItem = submenu.exec(item);
    if (rightClickItem && rightClickItem->text().contains("Delete"))
    {
        ui->listWidget->takeItem(ui->listWidget->indexAt(pos).row());
        ourColors.take(ui->listWidget->indexAt(pos).row());
    }
}

void MainWindow::ProvideContextMenu(const QPoint &pos)
{
    QPoint item = ui->listWidget->mapToGlobal(pos);
    QMenu submenu;
    submenu.addAction("Delete");
    submenu.addAction("Copy");

    QAction* rightClickItem = submenu.exec(item);
    if (rightClickItem && rightClickItem->text().contains("Delete"))
    {
        ui->listWidget->takeItem(ui->listWidget->indexAt(pos).row());
        ourColors.take(ui->listWidget->indexAt(pos).row());
    }

    if (rightClickItem && rightClickItem->text().contains("Copy"))
    {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(ui->listWidget->item(ui->listWidget->indexAt(pos).row())->text());
    }
}



void MainWindow::on_actionNew_Empty_Color_List_triggered()
{
    ourColors.clear();   // clear our colors
    ui->listWidget->clear();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (closing)
    {
        event->accept();
    }
    else
    {
        this->hide();
        event->ignore();
    }
}

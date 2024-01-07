#include "mainwindow.h"
#include "ui_mainwindow.h"

QColor getInvertedColour(QColor color)
{
    auto getInvert = [=](int c) -> int
    {
        if (c < 96 || c > 160)
            return 255 - c;
        else if (c < 128)
            return 255;
        else
            return 0;
    };

    color.setRed(getInvert(color.red()));
    color.setGreen(getInvert(color.green()));
    color.setBlue(getInvert(color.blue()));
    return color;
}

//

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
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


void MainWindow::on_actionAdd_New_Colour_triggered()
{
    // Show if hidden
    if (!this->isVisible())
    {
        this->show();
    }

    QColor colour = QColorDialog::getColor(Qt::white, this, "Select Colour");
    QString colourName = colour.name();

    QListWidgetItem* newItem = new QListWidgetItem(colourName);
    newItem->setBackgroundColor(colour);
    newItem->setTextColor(getInvertedColour(colour));

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
    ourColors.insert(insertRow, colourName);

    // Copy the colour name to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(colourName);


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
}

void MainWindow::on_listWidget_itemChanged(QListWidgetItem *item)
{
    item->setBackgroundColor(item->text());
}


void MainWindow::on_actionLoad_Colours_From_File_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Colours File"), "",
                                                    tr("Colours File (*.clrs);;All Files (*)"));


    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
        return;

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_5);


    ourColors.clear();   // clear our colours
    ui->listWidget->clear();

    in >> ourColors;


    QMap<int, QString>::iterator i;
    for (i = ourColors.begin(); i != ourColors.end(); i++)
    {
        currentRow = i.key();
        currentName = i.value();
        updateInterface();
    }
}


void MainWindow::updateInterface()
{
    ui->listWidget->addItem(currentName);
    QListWidgetItem* curItem = ui->listWidget->item(ui->listWidget->count()-1);
    curItem->setText(currentName);
    curItem->setBackgroundColor(curItem->text());
    curItem->setTextColor(getInvertedColour(QColor(curItem->text())));
    curItem->setFlags(curItem->flags()
                          .setFlag(Qt::ItemIsEditable, false));
}


void MainWindow::on_actionSave_Colours_to_File_triggered()
{
    refreshOurColorsFromUI();
    qDebug() << "Number of colours being saved:" << ourColors.size();

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Colours"), "",
                                                    tr("Save Colours (*.clrs);;All Files (*)"));


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
    QColor colour = QColorDialog::getColor(item->backgroundColor(), this, "Select Colour");
    item->setBackgroundColor(colour);
    item->setTextColor(getInvertedColour(colour));
    item->setText((QString)colour.name());

    // Copy the colour name to the clipboard
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(colour.name());
}


void MainWindow::on_actionAbout_Pigment_triggered()
{
    QMessageBox aboutBox;
    aboutBox.setText("Shade Saver; keep track of your colours!");
    aboutBox.setInformativeText("Jessica Leyba, 2024, GPL v3, see license for more information");
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


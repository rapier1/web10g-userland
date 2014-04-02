#include "mainwindow.hpp"
#include "statswindow.hpp"
#include "varwindow.hpp"
#include "estats_itemdelegate.hpp"
#include "conn_tablemodel.hpp"
#include "var_tablemodel.hpp"
#include <QApplication>                                                                   
#include <QCloseEvent>
#include <QHeaderView>
#include <QKeyEvent>
#include <QStatusBar>
#include <QTableView>
#include <QTimer>
#include <QVBoxLayout>
#include <QSettings>

#include <iostream>

extern "C" {                                                                              
#include "estats/estats.h"                                                                
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{ 
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

    estats::Check(estats_nl_client_init(&nl_client));

    model = new ConnTableModel(this, nl_client);

    createWidgets();
    createLayout();
    createConnections();

    setWindowTitle(QApplication::applicationName());
    statusBar()->showMessage(tr("Ready"), 0);

    QSettings settings;
    restoreGeometry(settings.value("MainWindow/geometry").toByteArray());

    update();
}


void MainWindow::createWidgets()
{
    tableView = new QTableView;
//    tableView->setModel(proxyModel);
    tableView->setModel(model);
    tableView->setItemDelegate(new EstatsItemDelegate(this));
    tableView->verticalHeader()->setDefaultAlignment(
            Qt::AlignVCenter|Qt::AlignRight);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setShowGrid(false);
    tableView->setSortingEnabled(TRUE);
    
}

void MainWindow::createLayout()
{ 
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void MainWindow::createConnections()
{
    connect(tableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&,
                                    const QItemSelection&)),
            this, SLOT(testSelectionChanged()));
    connect(tableView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(openStatsWindow()));
}

void MainWindow::openStatsWindow()
{
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndex currIndex = selectionModel->currentIndex();

    int currRow = currIndex.row();

    QStandardItem *cidItem = model->item(currRow, 0);
    QVariant cidVar = cidItem->data(Qt::DisplayRole);
    int res = cidVar.toInt();
/*
    varwin = new VarWindow(this, nl_client, res);
    varwin->setMinimumSize(400,600);
    varwin->show();
*/
    statswin = new StatsWindow(this, nl_client, res);
//    statswin->setMinimumSize(800,600);
    statswin->show();
}

void MainWindow::testSelectionChanged()
{
    std::cout << "selection changed\n";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
//    event->accept();
    QSettings settings;
    settings.setValue("MainWindow/geometry", saveGeometry());
    QWidget::closeEvent(event);
}

void MainWindow::update()
{
    model->update();
    tableView->resizeColumnsToContents();

    statusBar()->showMessage(tr("Viewing %n connections", "", model->rowCount()), 0);
}














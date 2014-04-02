#include "varwindow.hpp"
#include "estats_itemdelegate.hpp"
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

VarWindow::VarWindow(QWidget *parent, estats_nl_client *nl_client, int cid)
    : QMainWindow(parent)
{ 
    this->nl_client = nl_client;
    this->cid = cid;

    model = new VarTableModel(this, nl_client, cid);

    createWidgets();
    createLayout();
    createConnections();

    setWindowTitle(QApplication::applicationName());
//    statusBar()->showMessage(tr("Ready"), 0);

    QSettings settings;
    restoreGeometry(settings.value("VarWindow/geometry").toByteArray());

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}


void VarWindow::createWidgets()
{
    tableView = new QTableView;
//    tableView->setModel(proxyModel);
    tableView->setModel(model);
    tableView->setItemDelegate(new EstatsItemDelegate(this));
    tableView->verticalHeader()->setVisible(false);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->setShowGrid(false);
}

void VarWindow::createLayout()
{ 
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tableView);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);
}


void VarWindow::createConnections()
{
/*
   connect(tableView->selectionModel(),
            SIGNAL(selectionChanged(const QItemSelection&,
                                    const QItemSelection&)),
            this, SLOT(testSelectionChanged()));
    connect(tableView, SIGNAL(clicked(const QModelIndex&)),
            this, SLOT(openStatsWindow()));
*/
}

void VarWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    QSettings settings;
    settings.setValue("VarWindow/geometry", saveGeometry());
//    QWidget::closeEvent(event);
}

void VarWindow::update()
{
    model->update();
    tableView->resizeColumnsToContents();
}














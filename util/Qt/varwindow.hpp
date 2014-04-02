#ifndef VARWINDOW_HPP
#define VARWINDOW_HPP

#include <QMainWindow>

extern "C" {                                                                               
#include "estats/estats.h"                                                                 
}

class QTableView;
class VarTableModel;
//class ProxyModel;

class VarWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VarWindow(QWidget *parent=0, estats_nl_client *nl_client=NULL, int cid=-1);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void update();

private:
    void createWidgets();
    void createLayout();
    void createConnections(); // QT slot connections; need to change name

    estats_nl_client *nl_client;
    int cid;
    
    QTableView *tableView;
    VarTableModel *model;
//    ProxyModel *proxyModel;
};

#endif // MAINWINDOW_HPP

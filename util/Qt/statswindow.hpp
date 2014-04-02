#ifndef STATSWINDOW_HPP
#define STATSWINDOW_HPP

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include "graph.hpp"

extern "C" {                                                                               
#include "estats/estats.h"                                                                 
}

class QTableView;
class VarTableModel;
//class ProxyModel;
class QGraphicsEllipseItem;
class QLCDNumber;
class Graph;

class StatsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StatsWindow(QWidget *parent=0, estats_nl_client *nl_client=NULL, int cid=-1);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void update();
    void update_graph_vars();

private:
    void createWidgets();
    void createProxyWidgets();
    void createLayout();
    void createCentralWidget(); 
    void createConnections();
    
    estats_nl_client *nl_client;
    int cid;
    
    Graph *graph;
    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsEllipseItem *dishItem;
    QGraphicsRectItem *graphItem;
    VarTableModel *varModel;
    QTableView *varTableView;
    QTableView *varTableView2;
    QWidget *fake;
    QLCDNumber *whatLCD;
    QGraphicsRectItem *triRect;
    QGraphicsProxyWidget *varTableViewProxy;
    QGraphicsProxyWidget *varTableViewProxy2;
    QGraphicsProxyWidget *dishItemProxy;
    QGraphicsProxyWidget *triRectProxy;
    QGraphicsProxyWidget *whatLCDProxy;
    QGraphicsProxyWidget *fakeProxy;
    QRectF *rect;
//    ProxyModel *proxyModel;
};

#endif // STATSWINDOW_HPP

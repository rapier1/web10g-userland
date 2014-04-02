#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "EstatsView.hpp"
#include <QMainWindow>

class QTableView;
class ConnTableModel;
//class ProxyModel;
class VarWindow;
class StatsWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void update();
    void openStatsWindow();
    void testSelectionChanged();

private:
    void createWidgets();
    void createLayout();
    void createConnections(); // QT slot connections; need to change name

//    estats_agent *agent;

    estats_nl_client *nl_client;
    
    QTableView *tableView;
    ConnTableModel *model;
//    ProxyModel *proxyModel;
    VarWindow *varwin;
    StatsWindow *statswin;
};

#endif // MAINWINDOW_HPP

#include "statswindow.hpp"
#include "estats_itemdelegate.hpp"
#include "var_tablemodel.hpp"

#include <QApplication>

#include <QGraphicsScene>
#include <QTableView>

#include <iostream>

const int GraphSize = 350;
const int Margin = 20;

StatsWindow::StatsWindow(QWidget *parent, estats_nl_client *nl_client, int cid)
    : QMainWindow(parent)
{
    this->nl_client = nl_client;
    this->cid = cid;
/*
    scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, 800, 600);
*/
    createWidgets();
    createProxyWidgets();
    createLayout();
    createCentralWidget();
    createConnections();

    setWindowTitle(QApplication::applicationName());

//    QSettings settings;
//    restoreGeometry(settings.value("StatsWindow/geometry").toByteArray());
std::cout << "I'm here\n";
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

void StatsWindow::createWidgets()
{
    varModel = new VarTableModel(this, nl_client, cid);
    varTableView = new QTableView;

    varTableView->setModel(varModel);
    varTableView->setItemDelegate(new EstatsItemDelegate(this));
    varTableView->verticalHeader()->setVisible(false);
    varTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    varTableView->setShowGrid(false);
    varTableView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    varTableView->setMinimumHeight(400);
    varTableView->setMinimumWidth(400);
//    varTableView->setSortingEnabled(TRUE);

    scene = new QGraphicsScene;
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(0, 0, 800, 400);

/*
    dishItem = new QGraphicsEllipseItem;
    dishItem->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    dishItem->setPen(QPen(QColor("brown"), 2.5));
    dishItem->setBrush(Qt::white);
    dishItem->setRect(100, 50, 600, 300);

    scene->addItem(dishItem);
*/
    graphItem = new QGraphicsRectItem;
    graphItem->setFlags(QGraphicsItem::ItemClipsChildrenToShape);
    graphItem->setPen(QPen(QColor("brown"), 2.5));
    graphItem->setBrush(Qt::white);
    graphItem->setRect(100, 50, 600, 300);

    scene->addItem(graphItem);

    graph = new Graph(graphItem->boundingRect(), graphItem);
//    scene->addItem(graph);

//    QGraphicsView view(&scene);
//    view.setFrameStyle(0);

//    QRectF *rect = new QRectF(100,50,600,300);
//    scene->addWidget(rect);
    
    view = new QGraphicsView(scene);
    view->setRenderHints(QPainter::Antialiasing|
                         QPainter::TextAntialiasing);
    view->setBackgroundBrush(QColor("SteelBlue"));
    view->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    
}

void StatsWindow::createProxyWidgets()
{
//    varTableViewProxy = scene->addWidget(varTableView);
//    varTableViewProxy2 = scene->addWidget(varTableView2);
    
//    varTableViewProxy->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//    triRectProxy = scene->addWidget(triRect); 
//    whatLCDProxy = scene->addWidget(whatLCD);
//    fakeProxy = scene->addWidget(fake);
}

void StatsWindow::createLayout()
{
    QHBoxLayout *layout = new QHBoxLayout;

    layout->addWidget(varTableView);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);
    setCentralWidget(widget);


/*
    QGraphicsLinearLayout *leftLayout = new QGraphicsLinearLayout(Qt::Vertical);
    leftLayout->addItem(varTableViewProxy);
    leftLayout->insertStretch(1, 3);
    leftLayout->setContentsMargins(0,0,0,0);
    leftLayout->setSpacing(0);
*/
//    leftLayout->setStretchFactor(varTableViewProxy, 3);

//    QGraphicsLinearLayout *rightLayout = new QGraphicsLinearLayout(Qt::Vertical);
//    rightLayout->addItem(whatLCDProxy);
//    rightLayout->addItem(fakeProxy);
//    rightLayout->insertStretch(1, 3);
//    varTableViewProxy2->setGeometry(*rect);
/*
    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout; 
    layout->addItem(leftLayout);
    layout->setItemSpacing(0, 2*GraphSize + Margin);
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    
    layout->addItem(rightLayout);
    layout->insertStretch(1, 10);
*/

//    layout->addItem(varTableViewProxy, 0, 0);
//    layout->setAlignment(leftLayout, Qt::AlignLeft | Qt::AlignTop);
//    layout->setAlignment(varTableViewProxy, Qt::AlignLeft | Qt::AlignTop);

//    QGraphicsWidget *widget = new QGraphicsWidget;
//    widget->setLayout(layout);
//    scene->addItem(widget);



//    int width = qRound(layout->preferredWidth());
//    int height = GraphSize + (2 * Margin);
//    setMinimumSize(width, height);
//    scene->setSceneRect(0, 0, width, height);

//    scene->setSceneRect(0, 0, 800, 400);

}

void StatsWindow::createCentralWidget()
{
/*
    dishItem = new QGraphicsRectItem(varTableView2->width(), 0, 2*GraphSize, GraphSize, 0);

    dishItem->setBrush(Qt::white);
    scene->addItem(dishItem);

    

    view = new QGraphicsView(scene);
    view->setSceneRect(0, 0, view->frameSize().width(),view->frameSize().height());
    view->setRenderHints(QPainter::Antialiasing|
                         QPainter::TextAntialiasing);
    view->setBackgroundBrush(QColor("SteelBlue"));

    setCentralWidget(view);
*/
}

void StatsWindow::createConnections()
{
    connect(varTableView, SIGNAL(clicked(const QModelIndex&)),
	    this, SLOT(update_graph_vars()));
}

void StatsWindow::update_graph_vars()
{

    QItemSelectionModel *selectionModel = varTableView->selectionModel();
    QModelIndex currIndex = selectionModel->currentIndex();

    int currRow = currIndex.row();
    std::cout << "currRow: " << currRow << "\n";

    QStandardItem *varItem = varModel->item(currRow, 0);
    QVariant varVar = varItem->data(Qt::UserRole);
    int res = varVar.toInt();

    graph->update_vars(res);

    std::cout << "update_graph_vars\n";
    std::cout << res << "\n";
}

void StatsWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
    QSettings settings;
    settings.setValue("StatsWindow/geometry", saveGeometry());
//    disconnect(timer, SIGNAL(timeout()), this, SLOT(update()));
//    QWidget::closeEvent(event);
}

void StatsWindow::update()
{
    varModel->update();
    varTableView->resizeColumnsToContents();
}














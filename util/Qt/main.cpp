#include "mainwindow.hpp"
#include "EstatsView.hpp"
#include <QApplication>
#include <QPixmap>
#include <QIcon>

int main(int argc, char *argv[])
{
//    Q_INIT_RESOURCE(application);

    EApplication app(argc, argv);
    app.setApplicationName("estats-view");

//    QPixmap pix = QPixmap(":/E.png");
//    QIcon icon = QIcon(pix);
    QIcon icon = QIcon(":/E.png");

    app.setWindowIcon(icon);

    MainWindow window;
    window.setMinimumSize(700,300);
    window.show();
    return app.exec();
}

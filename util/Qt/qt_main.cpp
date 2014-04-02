//#include "mainwindow.hpp"
//#include "EstatsView.hpp"
#include <QtGui>
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QPixmap>
#include <QIcon>

int main(int argc, char *argv[])
{
//    EApplication app(argc, argv);
    QApplication app(argc, argv);
    app.setApplicationName("estats-view");

    QPixmap pix = QPixmap("./E.png");
    QIcon icon = QIcon(pix);

    app.setWindowIcon(icon);

//    MainWindow window;
    QMainWindow window;
    window.setMinimumSize(700,300);
    window.setWindowIcon(icon);
    window.show();
    return app.exec();
}

#include "MainWindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CadModelCore* model = new CadModelCore();

    MainWindow w;
    w.setModel(model);
    w.show();

    return a.exec();
}

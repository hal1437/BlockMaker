#include "MainWindow.h"
#include "SolidEditForm.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CadModelCore* model = new CadModelCore();

    MainWindow w;
    SolidEditForm s;
    s.setModel(model);
    w.setModel(model);

    w.installEventFilter(&s);
    s.show();
    w.show();

    return a.exec();
}

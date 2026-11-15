#include "InterfaceQt.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    InterfaceQt window;
    window.show();
    return app.exec();
}

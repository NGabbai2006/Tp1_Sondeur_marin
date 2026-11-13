#include "Tp1_Sondeur_marin.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Tp1_Sondeur_marin window;
    window.show();
    return app.exec();
}

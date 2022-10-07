#include "ui_display.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UI_display w;
    w.show();

    return a.exec();
}

#include "appmainform.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AppMainForm w;
    w.show();
    return a.exec();
}

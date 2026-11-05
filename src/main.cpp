#include "MainWindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("DeviceLink");
    QApplication::setOrganizationName("DeviceLink");
    QApplication::setApplicationVersion("0.1.0");

    MainWindow w;
    w.show();
    return app.exec();
}

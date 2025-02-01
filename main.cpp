#include "mainwindow.h"
#include <QApplication>

/**
 * @brief main entry point for the VocabBuilder application.
 */
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;
    window.setWindowTitle(QObject::tr("VocabBuilder"));
    window.show();
    return app.exec();
}

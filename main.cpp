#include "mainwindow.h"
#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    if (!translator.load(":/translations/zh_CN")) {
        qDebug() << "Failed to load translation file.";
    }
    a.installTranslator(&translator);

    MainWindow mainWindow;
    mainWindow.show();

    return a.exec();
}

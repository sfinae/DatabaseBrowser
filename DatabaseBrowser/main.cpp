
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include <QSplashScreen>

#include "DatabaseBrowser.h"
#include "ConnectDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	QApplication::addLibraryPath("../plugins");

    QString qtdir = getenv("QTDIR");
    qtdir+= "/translations";
	QTranslator translator;
	translator.load( "qt_ru.qm", qtdir);
    app.installTranslator(&translator);

    DatabaseBrowser browser;
    browser.show();

    app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
    return app.exec();
}

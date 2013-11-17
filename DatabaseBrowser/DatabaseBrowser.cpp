
#include <QApplication>
#include <QAction>
#include <QToolBar>
#include <QMenuBar>
#include <QMessageBox>
#include <QDockWidget>
#include <QTabWidget>
#include <QToolButton>
#include <QTextEdit>
#include <QActionGroup>
#include <QDir>
#include <QTranslator>
#include <QTextCodec>
#include <QPluginLoader>

#include "DatabaseBrowser.h"
#include "BaseViewer.h"
#include "DatabaseViewer.h"
#include "ConnectDialog.h"

DatabaseBrowser::DatabaseBrowser(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags)
{
    // set our translator
    appTranslator = new QTranslator;
    qApp->installTranslator(appTranslator);

    // add database viewer and make it dockable
    dbViewer = new DatabaseViewer;
    browser = new QDockWidget;
    browser->setWidget(dbViewer);
    addDockWidget(Qt::LeftDockWidgetArea, browser);

    // add text edit for messages and make it dockable
    textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    log = new QDockWidget;
    log->setWidget(textEdit);
    addDockWidget(Qt::BottomDockWidgetArea, log);

    toolBar = new QToolBar;

    createActions();
    createToolbar();
    createMenu();
    createStatusbar();

    //log->hide();

    // loadin all plugins
    loadPlugins();

    // main widget
    tabWidget = new QTabWidget();
    tabWidget->setTabsClosable(true);
    setCentralWidget(tabWidget);

    /*
    // close button for QTabWidget
    closeTabBtn = new QToolButton(tabWidget);
    closeTabBtn->setIcon(QIcon(":/DBMConverter/Resources/exit.png"));
    closeTabBtn->setAutoRaise(true);
    closeTabBtn->setShortcut(tr("Ctrl+F4"));
    tabWidget->setCornerWidget(closeTabBtn, Qt::TopRightCorner);
    //closeTabBtn->hide();
    */

    setWindowIcon(QIcon(":/DBMConverter/Resources/commonIco.png"));
    setWindowState(Qt::WindowMaximized);

    //connect(closeTabBtn, SIGNAL(clicked()), this, SLOT(onCloseCurrentTab()));
    connect(tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(onCloseTab(int)));
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(onCurrentTabChanged(int)));
    connect(dbViewer, SIGNAL(signalTreeWidgetItemActivated(const DatabaseItem&)),
            this, SLOT(onTreeWidgetItemActivated(const DatabaseItem&)));
    connect(dbViewer, SIGNAL(signalTreeWidgetRefreshed()), this, SLOT(onTreeWidgetRefreshed()));

    retranslate();
}

DatabaseBrowser::~DatabaseBrowser()
{
    // empty
}

void DatabaseBrowser::loadPlugins()
{
    // make plugin path
    QString path = qApp->applicationDirPath() + "/../plugins";
    QDir pluginDirectory(path);

    // set only files (it mean *.so on Unix, *.dynlib on MacOsX, .dll on Windows)
    QStringList fileNames = pluginDirectory.entryList(QDir::Files);

    foreach(const QString &file, fileNames)
    {
        QPluginLoader pluginLoader(pluginDirectory.absoluteFilePath(file));

        // get a plugin
        QObject *plugin = pluginLoader.instance();

        if (plugin)
        {
            // check interface
            BaseViewer *baseViewer = qobject_cast<BaseViewer*>(plugin);

            if (baseViewer)
            {
                vector.push_back(baseViewer);
            }
        }
    }

    // add plugins to menu and toolbar
    addPlugins();
}

void DatabaseBrowser::addPlugins()
{
    pluginActionGroup = new QActionGroup(this);
    connect(pluginActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(onPluginAction(QAction*)));

    for (int i = 0; i < vector.size(); ++i)
    {
        QAction *action = new QAction(vector.at(i)->icon(), vector.at(i)->text(), this);
        action->setStatusTip(vector.at(i)->statusTip());
        action->setData(i);
        pluginActionGroup->addAction(action);

        menuPlugins->addAction(action);
        toolBar->addAction(action);
    }
}

void DatabaseBrowser::onPluginAction(QAction* action)
{
    BaseViewer *bv = vector.at(action->data().toInt())->clone();

    connect(bv, SIGNAL(textMessage(const QString&, BaseViewer::message)),
            this, SLOT(onTextEditAddString(const QString&, BaseViewer::message)));

    int index = tabWidget->addTab(bv, makeTabText(bv));
    tabWidget->setCurrentIndex(index);

    DatabaseItem item = dbViewer->currentDatabaseItem();
    if (item.isValid())
    {
        changeTabData(bv, item, index);
    }
}

QString DatabaseBrowser::makeTabText(BaseViewer *baseViewer) const
{
    QString tabText = baseViewer->text();
    QString shortName = dbViewer->shortName();
    if (!shortName.isEmpty())
    {
        tabText.append(": " + shortName);
    }

    return tabText;
}

void DatabaseBrowser::createActions()
{
    actionAddConnection = new QAction(this);
    actionAddConnection->setIcon(QIcon(":/DBMConverter/Resources/add_32x32.png"));
    connect(actionAddConnection, SIGNAL(triggered()), this, SLOT(onAddConnection()));

    actionRemoveConnection = new QAction(this);
    actionRemoveConnection->setIcon(QIcon(":/DBMConverter/Resources/remove_32x32.png"));
    connect(actionRemoveConnection, SIGNAL(triggered()), this, SLOT(onRemoveConnection()));

    actionQuit = new QAction(this);
    actionQuit->setIcon(QIcon(":/DBMConverter/Resources/door_32x32.png"));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(close()));

    actionViewBrowser = new QAction(this);
    actionViewBrowser->setCheckable(true);
    actionViewBrowser->setChecked(true);
    connect(actionViewBrowser, SIGNAL(triggered(bool)), browser, SLOT(setVisible(bool)));
    connect(browser, SIGNAL(visibilityChanged(bool)), actionViewBrowser, SLOT(setChecked(bool)));

    actionViewLog = new QAction(this);
    actionViewLog->setCheckable(true);
    actionViewLog->setChecked(true);
    connect(actionViewLog, SIGNAL(triggered(bool)), log, SLOT(setVisible(bool)));
    connect(log, SIGNAL(visibilityChanged(bool)), actionViewLog, SLOT(setChecked(bool)));

    actionViewToolbar = new QAction(this);
    actionViewToolbar->setCheckable(true);
    actionViewToolbar->setChecked(true);
    connect(actionViewToolbar, SIGNAL(triggered(bool)), toolBar, SLOT(setVisible(bool)));
}

void DatabaseBrowser::createToolbar()
{
    toolBar->addAction(actionAddConnection);
    toolBar->addAction(actionRemoveConnection);
    toolBar->addSeparator();
    toolBar->addAction(actionQuit);
    toolBar->addSeparator();

    addToolBar(toolBar);
}

void DatabaseBrowser::createMenu()
{
    menuConnect = new QMenu;
    menuConnect->addAction(actionAddConnection);
    menuConnect->addAction(actionRemoveConnection);

    menuPlugins = new QMenu;

    createLanguageMenu();

    menuBar()->addMenu(menuConnect);
    menuBar()->addMenu(menuPlugins);
    menuBar()->addMenu(languageMenu);
}

void DatabaseBrowser::createStatusbar()
{
    //progressBar = new QProgressBar;
    //progressBar->hide();

    statusBar();
    //statusBar()->addPermanentWidget(progressBar, 1);
}

void DatabaseBrowser::onAddConnection()
{
    static int connectionNumber = 1;
    ConnectDialog dlg(connectionNumber++);
    if (dlg.exec() == QDialog::Accepted)
    {
        dbViewer->addDatabase(dlg.database());
        actionRemoveConnection->setEnabled(true);
        onTextEditAddString(tr("New connection added successfully"), BaseViewer::Message);
    }
}

void DatabaseBrowser::onRemoveConnection()
{
    if (QMessageBox::warning(this, 0, tr("Delete connection %1?").arg(dbViewer->currentDbCaption()),
        QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
    {
        dbViewer->removeActiveDatabase();

        if (dbViewer->isEmpty())
        {
            actionRemoveConnection->setEnabled(false);
        }

        onTextEditAddString(tr("Connection removed"), BaseViewer::Message);
    }
}

void DatabaseBrowser::onCloseTab(int tabIndex)
{
    QWidget* widget = tabWidget->widget(tabIndex);
    tabWidget->removeTab(tabIndex);
    widget->deleteLater();
}

void DatabaseBrowser::onCurrentTabChanged(int tabIndex)
{
    QWidget *widget = tabWidget->widget(tabIndex);
    if (widget)
    {
        QHash<QWidget*, QTreeWidgetItem*>::const_iterator it = m_tabToTreeMap.find(widget);
        if (it != m_tabToTreeMap.constEnd())
        {
            dbViewer->setCurrentItem(*it);
        }
    }
}

void DatabaseBrowser::onTreeWidgetItemActivated(const DatabaseItem &item)
{
    int index = tabWidget->currentIndex();

    if (index < 0)
    {
        return;
    }

    BaseViewer* data = qobject_cast<BaseViewer*>(tabWidget->widget(index));

    if (data)
    {
        changeTabData(data, item, index);
    }
}

void DatabaseBrowser::changeTabData(BaseViewer *data,
                                 const DatabaseItem& item,
                                 const int tabIndex)
{
    m_tabToTreeMap.insert(data, dbViewer->currentItem());

    tabWidget->setTabText(tabIndex, makeTabText(data));
    data->onDatabaseItemActivated(item);
}

void DatabaseBrowser::onTreeWidgetRefreshed()
{
    int index = tabWidget->currentIndex();

    if (index < 0)
        return;

    BaseViewer* data = qobject_cast<BaseViewer*>(tabWidget->widget(index));

    if (data)
    {
        //data->onTreeWidgetRefreshed();
    }
}

void DatabaseBrowser::onTextEditAddString(const QString& text, BaseViewer::message mes)
{
	QString message;

	switch (mes)
	{
	case BaseViewer::Error:
		//textEdit->setTextColor(Qt::red);
		message = "<font color = \"#ff0000\">[ERROR]: </font>";
		break;

	case BaseViewer::Warning:
		//textEdit->setTextColor(Qt::magenta);
		message = "<font color = \"#0000ff\">[WARNING]: </font>";
		break;

	case BaseViewer::Message:
		//textEdit->setTextColor(Qt::blue);
		message = "<font color = \"#00ff00\">[MESSAGE]: </font>";
		break;

	default:
		//textEdit->setTextColor(Qt::black);
		message = "<font color = \"#ffff00\">[UNDEFINED]: </font>";
		break;
	}

	textEdit->append(message + text);


}

void DatabaseBrowser::retranslate()
{
	// actions
    actionAddConnection->setText(tr("&Add connection"));
    actionAddConnection->setShortcut(tr("Ctrl+A"));
    actionAddConnection->setStatusTip(tr("Add database conneciton"));

    actionRemoveConnection->setText(tr("&Delete connection"));
    actionRemoveConnection->setShortcut(tr("Ctrl+D"));
    actionRemoveConnection->setStatusTip(tr("Delete current database connection"));

    actionQuit->setText(tr("&Quit"));
	actionQuit->setShortcut(tr("Ctrl+Q"));
    actionQuit->setStatusTip(tr("Quit"));

    actionViewBrowser->setText(tr("Database"));
    actionViewLog->setText(tr("Messages"));
    actionViewToolbar->setText(tr("Toolbox"));


	// toolbar
    toolBar->setWindowTitle(tr("Toolbox"));

	// menu
    menuConnect->setTitle(tr("&Connection"));
    menuPlugins->setTitle(tr("&Plugins"));
    languageMenu->setTitle(tr("&Language"));

	// dock widget
    browser->setWindowTitle(tr("Database"));
    log->setWindowTitle(tr("Messages"));
}

void DatabaseBrowser::createLanguageMenu()
{
	languageMenu = new QMenu(this);

	languageActionGroup = new QActionGroup(this);
	connect(languageActionGroup, SIGNAL(triggered(QAction *)), this, SLOT(switchLanguage(QAction *)));

	QDir qmDir = QDir(QApplication::applicationDirPath());
	qmDir.cdUp();
	qmDir.cd("Translations");

    QStringList fileNames = qmDir.entryList(QStringList("converter_*.qm"));
	for (int i = 0; i < fileNames.size(); ++i)
	{
		QString locale = fileNames[i];
		locale.remove(0, locale.indexOf('_') + 1);
		locale.chop(3);

		QTranslator translator;
		translator.load(fileNames[i], qmDir.absolutePath());
        QString language = translator.translate("MainWindow", "English");

		QAction *action = new QAction(tr("%1").arg(language), this);
		action->setCheckable(true);
		action->setData(locale);

		languageMenu->addAction(action);
		languageActionGroup->addAction(action);

        if (language == "English")
        {
			action->setChecked(true);
        }
	}
}

void DatabaseBrowser::switchLanguage(QAction *action)
{
	QString locale = action->data().toString();

	QDir qmDir = QDir(QApplication::applicationDirPath());
	qmDir.cdUp();
	qmDir.cd("Translations");

	QString qmPath = qmDir.absolutePath();

    appTranslator->load("browser_" + locale + ".qm", qmPath);

	retranslate();
}

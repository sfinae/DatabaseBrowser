#ifndef DBMCONVERTER_H
#define DBMCONVERTER_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QHash>

#include "BaseViewer.h"

class QDockWidget;
class QAction;
class QTabWidget;
class QToolButton;
class QTextEdit;
class QActionGroup;
class QTranslator;
class QTreeWidgetItem;

class DatabaseViewer;


class DatabaseBrowser : public QMainWindow
{
	Q_OBJECT

public:
    DatabaseBrowser(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    virtual ~DatabaseBrowser();

private slots:
    void onCloseTab(int tabIndex);
    void onCurrentTabChanged(int tabIndex);
    void onAddConnection();
    void onRemoveConnection();
    void onPluginAction(QAction *);
    void switchLanguage(QAction *);

    void onTreeWidgetItemActivated(const DatabaseItem &item);
    void onTreeWidgetRefreshed();

    void onTextEditAddString(const QString&, BaseViewer::message);

private:
    void createActions();
    void createMenu();
    void createToolbar();
    void createStatusbar();
    void retranslate();
    void createLanguageMenu();
    void loadPlugins();
    void addPlugins();
    QString makeTabText(BaseViewer *baseViewer) const;
    void changeTabData(BaseViewer *data,
                       const DatabaseItem &item,
                       const int tabIndex);

    QHash<QWidget*, QTreeWidgetItem*> m_tabToTreeMap;
    DatabaseViewer *dbViewer;
    QDockWidget *browser;
    QTabWidget *tabWidget;

    QTextEdit *textEdit;
    QDockWidget *log;

    QAction *actionAddConnection;
    QAction *actionRemoveConnection;
    QAction *actionAddView;
    QAction *actionAddSqlView;
    QAction *actionExport;
    QAction *actionImport;
    QAction *actionQuit;
    QAction *actionViewBrowser;
    QAction *actionViewLog;
    QAction *actionViewToolbar;

    QToolButton *closeTabBtn;
    QActionGroup *languageActionGroup;
    QTranslator *appTranslator;

    QToolBar *toolBar;

    QMenu *menuConnect;
    QMenu *menuPlugins;
    QMenu *languageMenu;

    QVector<BaseViewer*> vector;
    QActionGroup *pluginActionGroup;
};

#endif // DBMCONVERTER_H

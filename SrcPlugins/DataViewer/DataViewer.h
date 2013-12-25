#ifndef DATA_VIEWER_H
#define DATA_VIEWER_H

#include <QScopedPointer>

#include "BaseViewer.h"

class QTableView;
class QSplitter;
class QStandardItemModel;
class QSqlTableModel;
class QEvent;
class QAction;

class DatabaseViewer;
class TableView;

class DataViewer : public BaseViewer
{
    Q_OBJECT
    Q_INTERFACES(BaseViewer)
    Q_PLUGIN_METADATA(IID "com.michael.PluginExample.BaseViewer")

public:
    DataViewer();
    virtual ~DataViewer();

    virtual bool eventFilter(QObject *watched, QEvent *event);

    virtual BaseViewer* clone() const;

    virtual void onDatabaseItemRemoved();
    virtual void onDatabaseItemActivated(const DatabaseItem &item);

    virtual QIcon icon() const;
    virtual QString statusTip() const;
    virtual QString text() const;

protected:
    virtual void retranslate();

private:
    void createModels();

    QSplitter *splitter;
    TableView *tableShema;
    TableView *m_tableData;

    QAction *m_timestampAction;

    QScopedPointer<QStandardItemModel> m_shemaModel;
    QScopedPointer<QSqlTableModel> m_sqlDataModel;
};

#endif // DATA_VIEWER_H

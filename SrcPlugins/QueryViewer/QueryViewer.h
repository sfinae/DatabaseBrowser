
#ifndef QUERY_VIEWER_H
#define QUERY_VIEWER_H

#include <QScopedPointer>

#include "BaseViewer.h"

class QTableView;
class QTextEdit;
class QPushButton;
class QSplitter;
class TableView;
class QGroupBox;
class QSqlQueryModel;

class QueryViewer : public BaseViewer
{
    Q_OBJECT
    Q_INTERFACES(BaseViewer)
    Q_PLUGIN_METADATA(IID "com.michael.PluginExample.BaseViewer")

public:
    QueryViewer();
    virtual ~QueryViewer();

    virtual BaseViewer* clone() const;

    virtual void onDatabaseItemRemoved();
    virtual void onDatabaseItemActivated(const DatabaseItem &item);
    virtual bool hasValidDb() const;

    virtual QIcon icon() const;
    virtual QString statusTip() const;
    virtual QString text() const;

protected:
    virtual void retranslate();

private slots:
    void onSubmitClicked();

private:
    void configModel(const QString &queryText);
    void executeTransaction(const QStringList &sqlQuerys);
    void showQueryError(const QSqlQuery &query);

    QScopedPointer<QSqlQueryModel> m_queryModel;
    TableView *sqlView;
    QTextEdit *sqlEdit;
    QPushButton *pbClean;
    QPushButton *pbSubmit;
    QSplitter *splitter;
    QGroupBox *groupBox;
    QSqlDatabase database;
};

#endif // QUERY_VIEWER_H

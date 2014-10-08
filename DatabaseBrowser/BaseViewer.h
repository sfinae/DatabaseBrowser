#ifndef BASE_VIEWER_H
#define BASE_VIEWER_H

#include <QIcon>
#include <QSqlDatabase>

#include "abstractwidget.h"

class DatabaseItem
{
public:
    enum ItemType
    {
        None,
        Table,
        View
    };

    DatabaseItem(const QSqlDatabase &database)
        : m_database(database)
        , m_type(None)
    {
        // empty
    }

    DatabaseItem(const QSqlDatabase &database,
                 const ItemType itemType,
                 const QString &itemName)
        : m_database(database)
        , m_type(itemType)
        , m_value(itemName)
    {
        // empty
    }

    bool isValid() const
    {
        return m_database.isValid();
    }

    QSqlDatabase m_database;
    ItemType m_type;
    QString m_value;
};

class BaseViewer : public AbstractWidget
{
    Q_OBJECT

public:
    enum message{Error, Warning, Message};

    BaseViewer(QWidget *parent = 0, Qt::WindowFlags flags = 0)
        : AbstractWidget(parent, flags)
    {
    }

    virtual ~BaseViewer()
    {
        // empty
    }

    // new instance of ourserlf
    virtual BaseViewer* clone() const = 0;

    virtual void onDatabaseItemRemoved() = 0;
    virtual void onDatabaseItemActivated(const DatabaseItem &item) = 0;
    virtual bool hasValidDb() const = 0;

    // return a icon to menu and toolbar
    virtual QIcon icon() const
    {
        return QIcon();
    }

    // text to menu and toolbar
    virtual QString text() const
    {
        return QString();
    }

    // return tip to status bar
    virtual QString statusTip() const
    {
        return QString();
    }

protected:
    // translate all strings
    virtual void retranslate() = 0;

signals:
    // user notification
    void textMessage(const QString& text, BaseViewer::message = Message);
};

Q_DECLARE_INTERFACE(BaseViewer, "com.michael.PluginExample.BaseViewer")

#endif // BASE_VIEWER_H

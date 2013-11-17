#ifndef DATABASE_VIEWER_H
#define DATABASE_VIEWER_H

#include <QWidget>
#include <QSqlDatabase>

#include "BaseViewer.h"

class QTreeWidget;
class QTreeWidgetItem;

class DatabaseViewer : public QWidget
{
    Q_OBJECT

public:
    DatabaseViewer(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    void addDatabase(const QSqlDatabase &database);
    void removeActiveDatabase();

    QSqlDatabase currentDatabase() const;
    DatabaseItem currentDatabaseItem() const;
    QString shortName() const;
    QString activeConnection() const;
    QString currentDbCaption() const;
    bool isEmpty() const;
    QTreeWidgetItem* currentItem() const;
    void setCurrentItem(QTreeWidgetItem *item);

public slots:
    void refresh();

private slots:
    void onTreeWidgetItemActivated(QTreeWidgetItem *item, int column);

signals:
    void signalTreeWidgetItemActivated(const DatabaseItem &item);
    void signalTreeWidgetRefreshed();

protected:
    virtual void changeEvent(QEvent *event);

private:
    void insertGroupItems(QTreeWidgetItem *rootItem,
                          const QStringList &items,
                          const QString &groupName,
                          const DatabaseItem::ItemType type);
    QTreeWidgetItem* topLevelItem(QTreeWidgetItem *item) const;

    QString dbCaption(const QSqlDatabase &db) const;
    void setActive(QTreeWidgetItem *item);
    void retranslate();

    QTreeWidget *treeWidget;
    QString activeDB;
    QTreeWidgetItem *m_activeDbItem;

    QAction *actionRefresh;
};

#endif // DATABASE_VIEWER_H

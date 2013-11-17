

#include <QTreeWidget>
#include <QVBoxLayout>
#include <QAction>
#include <QHeaderView>
#include <QSqlDatabase>
#include <QTreeWidgetItem>
#include <QEvent>

#include "DatabaseViewer.h"

DatabaseViewer::DatabaseViewer(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{

    QVBoxLayout *layout = new QVBoxLayout(this);

    treeWidget = new QTreeWidget(this);
    treeWidget->setSortingEnabled(true);
    treeWidget->sortByColumn(0, Qt::AscendingOrder);
    treeWidget->setHeaderLabel(tr(""));
    //treeWidget->header()->setResizeMode(QHeaderView::Stretch);
    //treeWidget->header()->hide();


    actionRefresh = new QAction(treeWidget);
    connect(actionRefresh, SIGNAL(triggered()), SLOT(refresh()));
    treeWidget->addAction(actionRefresh);
    treeWidget->setContextMenuPolicy(Qt::ActionsContextMenu);

    layout->addWidget(treeWidget);

    connect(treeWidget, SIGNAL(itemActivated(QTreeWidgetItem*, int)),
            this, SLOT(onTreeWidgetItemActivated(QTreeWidgetItem*, int)));

    retranslate();
    //refresh();
}

void DatabaseViewer::addDatabase(const QSqlDatabase &database)
{
    QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget);
    root->setText(0, dbCaption(database));
    root->setData(0, Qt::UserRole, database.connectionName());
    root->setData(0, Qt::UserRole + 1, DatabaseItem::None);

    if (database.isOpen())
    {
        QStringList tables = database.tables();
        if (!tables.isEmpty())
        {
            insertGroupItems(root, tables, tr("Tables"), DatabaseItem::Table);
        }

        QStringList views = database.tables(QSql::Views);
        if (!views.isEmpty())
        {
            insertGroupItems(root, views, tr("Views"), DatabaseItem::View);
        }

        QStringList sysTables = database.tables(QSql::SystemTables);
        if (!sysTables.isEmpty())
        {
            insertGroupItems(root, sysTables, tr("System tables"), DatabaseItem::Table);
        }
    }

    if (activeDB.isEmpty())
    {
        setActive(root);
    }
}

void DatabaseViewer::removeActiveDatabase()
{
    QSqlDatabase::removeDatabase(activeDB);

    treeWidget->takeTopLevelItem(treeWidget->indexOfTopLevelItem(m_activeDbItem));
}

void DatabaseViewer::insertGroupItems(QTreeWidgetItem *rootItem,
                                      const QStringList &items,
                                      const QString &groupName,
                                      const DatabaseItem::ItemType type)
{
    if (!items.isEmpty())
    {
        QTreeWidgetItem *groupItem = new QTreeWidgetItem(rootItem);
        groupItem->setText(0, groupName);
        groupItem->setData(0, Qt::UserRole + 1, DatabaseItem::None);

        for (int i = 0; i < items.count(); ++i)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(groupItem);
            item->setText(0, items.at(i));
            item->setData(0, Qt::UserRole + 1, type);
        }
    }
}

void DatabaseViewer::refresh()
{
    treeWidget->clear();
    QStringList connectionNames = QSqlDatabase::connectionNames();

    bool gotActiveDB = false;
    for (int i = 0; i < connectionNames.count(); ++i)
	{
        QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget);
        QSqlDatabase db = QSqlDatabase::database(connectionNames.at(i), false);
        root->setText(0, dbCaption(db));
		root->setData(0, Qt::UserRole, i);

        if (connectionNames.at(i) == activeDB)
		{
            gotActiveDB = true;
            setActive(root);
        }

        if (db.isOpen())
		{
            QStringList tables = db.tables();
            for (int t = 0; t < tables.count(); ++t)
			{
                QTreeWidgetItem *table = new QTreeWidgetItem(root);
                table->setText(0, tables.at(t));
            }
        }
    }

    if (!gotActiveDB)
	{
        activeDB = connectionNames.value(0);
        setActive(treeWidget->topLevelItem(0));
    }

	emit signalTreeWidgetRefreshed();
}

QTreeWidgetItem* DatabaseViewer::topLevelItem(QTreeWidgetItem *item) const
{
    QTreeWidgetItem *output = 0;

    int index = treeWidget->indexOfTopLevelItem(item);

    if (index < 0)
    {
        while (item->parent())
        {
            item = item->parent();
        }

        output = item;
    }
    else
    {
        output = treeWidget->topLevelItem(index);
    }

    return output;
}

QString DatabaseViewer::dbCaption(const QSqlDatabase &db) const
{
    QString caption = db.driverName();
    caption.append("-");

    if (!db.userName().isEmpty())
    {
        caption.append(db.userName() + "-");
    }

    caption.append(db.databaseName());

    return caption;
}

void DatabaseViewer::setActive(QTreeWidgetItem *item)
{
    if (!item)
    {
        return;
    }

    QTreeWidgetItem *topItem = topLevelItem(item);

    if (topItem)
    {
        QFont font;
        for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
        {
            font = treeWidget->topLevelItem(i)->font(0);
            if (font.bold())
            {
                font.setBold(false);
                treeWidget->topLevelItem(i)->setFont(0, font);
            }
        }

        font = topItem->font(0);
        font.setBold(true);
        topItem->setFont(0, font);

        m_activeDbItem = topItem;
        activeDB = topItem->data(0, Qt::UserRole).toString();
    }
}

void DatabaseViewer::onTreeWidgetItemActivated(QTreeWidgetItem *item, int /* column */)
{
    if (!item || !item->parent())
    {
        return;
    }

    DatabaseItem::ItemType type = (DatabaseItem::ItemType)item->data(0, Qt::UserRole + 1).toInt();

    if (DatabaseItem::None == type)
    {
        return;
    }

    setActive(item);
    QSqlDatabase db = currentDatabase();

    DatabaseItem dbItem(db, type, item->text(0));
    emit signalTreeWidgetItemActivated(dbItem);
}

QSqlDatabase DatabaseViewer::currentDatabase() const
{
    return QSqlDatabase::database(activeDB);
}

DatabaseItem DatabaseViewer::currentDatabaseItem() const
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    if (!item)
    {
        return DatabaseItem(currentDatabase());
    }

    DatabaseItem::ItemType type = (DatabaseItem::ItemType)item->data(0, Qt::UserRole + 1).toInt();

    if (DatabaseItem::None != type)
    {
        QSqlDatabase db = currentDatabase();
        DatabaseItem dbItem(db, type, item->text(0));

        return dbItem;
    }

    return DatabaseItem(currentDatabase());
}

QString DatabaseViewer::shortName() const
{
    QString output;

    QTreeWidgetItem *item = treeWidget->currentItem();
    if (item && item->parent())
    {
        DatabaseItem::ItemType type = (DatabaseItem::ItemType)item->data(0, Qt::UserRole + 1).toInt();
        if (DatabaseItem::None != type)
        {
            QSqlDatabase curDb = currentDatabase();
            output += curDb.databaseName() + " " + item->text(0);
            return output;
        }
    }

    return output;
}

QString DatabaseViewer::activeConnection() const
{
    return activeDB;
}

QString DatabaseViewer::currentDbCaption() const
{
    QString caption;

    if (!activeDB.isEmpty())
    {
        QSqlDatabase db = QSqlDatabase::database(activeDB);
        caption = dbCaption(db);
    }

    return caption;
}

bool DatabaseViewer::isEmpty() const
{
    if (treeWidget->topLevelItemCount())
    {
        return false;
    }
    else
    {
        return true;
    }
}

QTreeWidgetItem* DatabaseViewer::currentItem() const
{
    QTreeWidgetItem *item = treeWidget->currentItem();

    /*DatabaseItem::ItemType type = (DatabaseItem::ItemType)item->data(0, Qt::UserRole + 1).toInt();
    if (DatabaseItem::None != type)
    {
        return item;
    }

    return 0;
    */
    return item;
}

void DatabaseViewer::setCurrentItem(QTreeWidgetItem *item)
{
    treeWidget->setCurrentItem(item);
}

void DatabaseViewer::retranslate()
{
    actionRefresh->setText(tr("Reload"));
}

void DatabaseViewer::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        retranslate();
    }
}

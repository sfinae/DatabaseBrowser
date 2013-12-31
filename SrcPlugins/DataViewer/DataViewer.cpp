
#include "DataViewer.h"

#include <QtPlugin>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QTabWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMenu>
#include <QDateTime>

namespace
{
    enum DataViewerTabIndex
    {
        Data = 0,
        Scheme
    };

    const char *c_dataTable = QT_TRANSLATE_NOOP("DataViewer", "Data %1");
    const char *c_schemeTable = QT_TRANSLATE_NOOP("DataViewer", "Scheme %1");
}

class DataViewerPrivate
{
public:
    void createModels();

    QTabWidget *m_tab;
    QTableView *m_tableShema;
    QTableView *m_tableData;

    QAction *m_timestampAction;

    QScopedPointer<QStandardItemModel> m_shemaModel;
    QScopedPointer<QSqlTableModel> m_sqlDataModel;
};

void DataViewerPrivate::createModels()
{
    m_shemaModel.reset(new QStandardItemModel(m_tableShema));
    m_shemaModel->insertColumns(0, 7);
}

DataViewer::DataViewer()
    : d_ptr(new DataViewerPrivate())
{
    d_ptr->m_tableShema = new QTableView(this);
    d_ptr->m_tableData = new QTableView(this);

    // context menu
    d_ptr->m_tableData->viewport()->installEventFilter(this);

    d_ptr->m_tab = new QTabWidget(this);
    d_ptr->m_tab->addTab(d_ptr->m_tableData, tr(::c_dataTable).arg(""));
    d_ptr->m_tab->addTab(d_ptr->m_tableShema, tr(::c_schemeTable).arg(""));

    QVBoxLayout *box = new QVBoxLayout;
    box->addWidget(d_ptr->m_tab);
    setLayout(box);

    // on right click shows timestamp
    d_ptr->m_timestampAction = new QAction(tr("Show timestamp"), this);

    d_ptr->createModels();
    retranslate();
}

DataViewer::~DataViewer()
{
    // empty
}

bool DataViewer::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton)
        {
            const QPoint globalMousePos = mouseEvent->globalPos();

            QTableView *tableView = d_ptr->m_tableData;
            Q_ASSERT(tableView);

            const QModelIndex tableModelIndex = tableView->indexAt(mouseEvent->pos());
            if (tableModelIndex.isValid())
            {
                QVariant data = tableView->model()->data(tableModelIndex);
                bool ok = false;
                int timestamp = data.toInt(&ok);
                if (ok)
                {
                    QMenu menu;
                    menu.addAction(d_ptr->m_timestampAction);
                    if (menu.exec(globalMousePos))
                    {
                        QMessageBox::information(this, QString(), QDateTime::fromTime_t(timestamp).toString());
                    }
                }
            }
        }
    }

    return BaseViewer::eventFilter(watched, event);
}

BaseViewer* DataViewer::clone() const
{
    return new DataViewer();
}

void DataViewer::onDatabaseItemActivated(const DatabaseItem &item)
{
    if ((DatabaseItem::Table != item.m_type) &&
            (DatabaseItem::View != item.m_type))
    {
        return;
    }

    // for table tableShema
    QSqlRecord record = item.m_database.record(item.m_value);

    d_ptr->m_shemaModel->removeRows(0, d_ptr->m_shemaModel->rowCount());
    d_ptr->m_shemaModel->insertRows(0, record.count());

    for (int i = 0; i < record.count(); ++i)
    {
        QSqlField field = record.field(i);

        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 0), field.name());
        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 1),
                              field.type() == QVariant::Invalid ? "???" : QString(QVariant::typeToName(field.type())));
        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 2),
                              (field.length() < 0) ? QVariant("???") : field.length());
        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 3),
                              (field.precision() < 0) ? QVariant("???") : field.precision());
        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 4),
                              field.requiredStatus() == QSqlField::Unknown ? "???" : QVariant(bool(field.requiredStatus())));
        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 5), field.isAutoValue());
        d_ptr->m_shemaModel->setData(d_ptr->m_shemaModel->index(i, 6), field.defaultValue());
    }

    d_ptr->m_tableShema->setModel(d_ptr->m_shemaModel.data());
    d_ptr->m_tableShema->setEditTriggers(QAbstractItemView::NoEditTriggers);
    d_ptr->m_tableShema->resizeColumnsToContents();

    d_ptr->m_tab->setTabText(::Scheme, tr(::c_schemeTable).arg(item.m_value));

    // for table tableData
    QSqlTableModel *modelData = new QSqlTableModel(0, item.m_database);
    modelData->setEditStrategy(QSqlTableModel::OnRowChange);
    modelData->setTable(item.m_value);
    modelData->select();

    d_ptr->m_tableData->setModel(modelData);
    d_ptr->m_tableData->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    d_ptr->m_tableData->resizeColumnsToContents();

    d_ptr->m_tab->setTabText(::Data, tr(::c_dataTable).arg(item.m_value));

    d_ptr->m_sqlDataModel.reset(modelData);
}

void DataViewer::onDatabaseItemRemoved()
{
    /*
    (*tableData)->setModel(0);
    tableData->setText("");
    (*tableShema)->setModel(0);
    tableShema->setText("");
    */
}

QIcon DataViewer::icon() const
{
    return QIcon(":/PluginExample/Resources/table_32x32.png");
}

QString DataViewer::text() const
{
    return tr("&Viewer");
}

QString DataViewer::statusTip() const
{
    return tr("Database viewer");
}

void DataViewer::retranslate()
{
    d_ptr->m_shemaModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    d_ptr->m_shemaModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    d_ptr->m_shemaModel->setHeaderData(2, Qt::Horizontal, tr("Length"));
    d_ptr->m_shemaModel->setHeaderData(3, Qt::Horizontal, tr("Precision"));
    d_ptr->m_shemaModel->setHeaderData(4, Qt::Horizontal, tr("Mandatory"));
    d_ptr->m_shemaModel->setHeaderData(5, Qt::Horizontal, tr("Automatic"));
    d_ptr->m_shemaModel->setHeaderData(6, Qt::Horizontal, tr("Default"));

    d_ptr->m_tab->setTabText(::Scheme, tr(::c_schemeTable).arg(""));
    d_ptr->m_tab->setTabText(::Data, tr(::c_dataTable).arg(""));
}

//Q_EXPORT_PLUGIN2(dataviewer, DataViewer)

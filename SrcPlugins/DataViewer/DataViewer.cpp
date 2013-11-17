
#include <QtPlugin>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QSplitter>
#include <QTableView>
#include <QVBoxLayout>

#include "DataViewer.h"
#include "TableView.h"



DataViewer::DataViewer()
{
    tableShema = new TableView;
    tableData = new TableView;

    splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(tableShema);
    splitter->addWidget(tableData);

    QVBoxLayout *box = new QVBoxLayout;
    box->addWidget(splitter);
    setLayout(box);

    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    createModels();
    retranslate();
}

DataViewer::~DataViewer()
{
    // empty
}

BaseViewer* DataViewer::clone() const
{
    return new DataViewer();
}

void DataViewer::createModels()
{
    m_shemaModel.reset(new QStandardItemModel(tableShema));
    m_shemaModel->insertColumns(0, 7);
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

    m_shemaModel->removeRows(0, m_shemaModel->rowCount());
    m_shemaModel->insertRows(0, record.count());

    for (int i = 0; i < record.count(); ++i)
    {
        QSqlField field = record.field(i);

        m_shemaModel->setData(m_shemaModel->index(i, 0), field.name());
        m_shemaModel->setData(m_shemaModel->index(i, 1),
                              field.type() == QVariant::Invalid ? "???" : QString(QVariant::typeToName(field.type())));
        m_shemaModel->setData(m_shemaModel->index(i, 2),
                              (field.length() < 0) ? QVariant("???") : field.length());
        m_shemaModel->setData(m_shemaModel->index(i, 3),
                              (field.precision() < 0) ? QVariant("???") : field.precision());
        m_shemaModel->setData(m_shemaModel->index(i, 4),
                              field.requiredStatus() == QSqlField::Unknown ? "???" : QVariant(bool(field.requiredStatus())));
        m_shemaModel->setData(m_shemaModel->index(i, 5), field.isAutoValue());
        m_shemaModel->setData(m_shemaModel->index(i, 6), field.defaultValue());
    }

    (*tableShema)->setModel(m_shemaModel.data());
    (*tableShema)->setEditTriggers(QAbstractItemView::NoEditTriggers);
    (*tableShema)->resizeColumnsToContents();
    tableShema->setText(tr("Sheme %1").arg(item.m_value));

    // for table tableData
    QSqlTableModel *modelData = new QSqlTableModel(0, item.m_database);
    modelData->setEditStrategy(QSqlTableModel::OnRowChange);
    modelData->setTable(item.m_value);
    modelData->select();

    //if (model->lastError().type() != QSqlError::NoError)
        //emit statusMessage(model->lastError().text());
    (*tableData)->setModel(modelData);
    (*tableData)->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    (*tableData)->resizeColumnsToContents();
    tableData->setText(tr("Data %1").arg(item.m_value));

    m_sqlDataModel.reset(modelData);

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
    m_shemaModel->setHeaderData(0, Qt::Horizontal, tr("Name"));
    m_shemaModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    m_shemaModel->setHeaderData(2, Qt::Horizontal, tr("Length"));
    m_shemaModel->setHeaderData(3, Qt::Horizontal, tr("Precision"));
    m_shemaModel->setHeaderData(4, Qt::Horizontal, tr("Mandatory"));
    m_shemaModel->setHeaderData(5, Qt::Horizontal, tr("Automatic"));
    m_shemaModel->setHeaderData(6, Qt::Horizontal, tr("Default"));

    tableShema->setText(tr("Sheme %1").arg(""));
    tableData->setText(tr("Data %1").arg(""));
}

//Q_EXPORT_PLUGIN2(dataviewer, DataViewer)

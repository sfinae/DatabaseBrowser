
#include <QtPlugin>
#include <QTableView>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlError>

#include "QueryViewer.h"
#include "TableView.h"
#include "Highlighter.h"


QueryViewer::QueryViewer()
    : m_queryModel(new QSqlQueryModel())
{
    sqlView = new TableView;
    (*sqlView)->setModel(m_queryModel.data());

    sqlEdit = new QTextEdit(this);
    Highlighter *highlighter = new Highlighter(sqlEdit->document());

    pbClean = new QPushButton(this);
    pbSubmit = new QPushButton(this);

    QHBoxLayout *pbLayout = new QHBoxLayout;
    pbLayout->addWidget(pbClean);
    pbLayout->addWidget(pbSubmit);
    pbLayout->addStretch();

    QVBoxLayout *groupBoxLayout = new QVBoxLayout;
    groupBoxLayout->addWidget(sqlEdit);
    groupBoxLayout->addLayout(pbLayout);

    groupBox = new QGroupBox(this);
    groupBox->setLayout(groupBoxLayout);

    splitter = new QSplitter(Qt::Vertical, this);
    splitter->addWidget(sqlView);
    splitter->addWidget(groupBox);

    QVBoxLayout *box = new QVBoxLayout;
    box->addWidget(splitter);
    setLayout(box);

    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 0);

    connect(pbClean, SIGNAL(clicked()), sqlEdit, SLOT(clear()));
    connect(pbSubmit, SIGNAL(clicked()), this, SLOT(onSubmitClicked()));

    retranslate();
}

QueryViewer::~QueryViewer()
{
    // empty
}

BaseViewer* QueryViewer::clone() const
{
    return new QueryViewer();
}

void QueryViewer::onDatabaseItemActivated(const DatabaseItem &item)
{
    if (!item.isValid())
    {
        return;
    }

    database = item.m_database;

    const QString query("SELECT * FROM " + item.m_value + ";");
    configModel(query);
    sqlEdit->setText(query);
}

void QueryViewer::onDatabaseItemRemoved()
{
    // todo
}

QIcon QueryViewer::icon() const
{
    return QIcon(":/PluginExample/Resources/edit_32x32.png");
}

QString QueryViewer::text() const
{
    return tr("&SQL edit");
}

QString QueryViewer::statusTip() const
{
    return tr("Sql edit");
}

void QueryViewer::onSubmitClicked()
{
    configModel(sqlEdit->toPlainText());
}

void QueryViewer::retranslate()
{
    sqlView->setText(tr("Data"));
    pbClean->setText(tr("Clean"));
    pbSubmit->setText(tr("Run"));
    groupBox->setTitle(tr("SQL Query"));
}

void QueryViewer::configModel(const QString &queryText)
{
    m_queryModel->clear();
    m_queryModel->setQuery(QSqlQuery(queryText, database));

    if (m_queryModel->lastError().type() != QSqlError::NoError)
    {
        emit textMessage(m_queryModel->lastError().text(), BaseViewer::Warning);
    }
    else if (m_queryModel->query().isSelect())
    {
        emit textMessage(tr("Sql query executed successfully"));
    }
    else
    {
        emit textMessage(tr("Query executed successfully, number affected rows: %1").arg(m_queryModel->query().numRowsAffected()));
    }
}

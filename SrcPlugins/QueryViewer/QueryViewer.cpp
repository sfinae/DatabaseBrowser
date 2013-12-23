
#include "QueryViewer.h"
#include "TableView.h"
#include "Highlighter.h"
#include "QueryParser.h"

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
#include <QDateTime>

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
    QString text = sqlEdit->toPlainText();
    //QStringList sqlQuerys = text.split(";", QString::SkipEmptyParts);
    QueryParser parser;
    QStringList sqlQuerys = parser.parse(text);

    QDateTime startTime = QDateTime::currentDateTime();

    if (sqlQuerys.size() > 1)
    {
        executeTransaction(sqlQuerys);
    }
    else
    {
        configModel(text);
    }

    const int executionTime = startTime.msecsTo(QDateTime::currentDateTime());
    emit textMessage(tr("Query execution time: %1 ms").arg(executionTime));
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

    showQueryError(m_queryModel->query());
}

void QueryViewer::executeTransaction(const QStringList &sqlQuerys)
{
    QSqlQuery query(database);
    QString lastSelectQuery;

    database.transaction();
    emit textMessage(tr("Transaction started"));

    for (int i = 0; i < sqlQuerys.size(); ++i)
    {
        bool success = query.exec(sqlQuerys.at(i));
        showQueryError(query);

        if (!success)
        {
            database.rollback();
            emit textMessage("Transaction reverted", BaseViewer::Warning);
            return;
        }

        if (query.isSelect())
        {
            lastSelectQuery = sqlQuerys.at(i);
        }
    }

    database.commit();
    emit textMessage("Transaction commited");

    configModel(lastSelectQuery);
}

void QueryViewer::showQueryError(const QSqlQuery &query)
{
    if (query.lastError().type() != QSqlError::NoError)
    {
        emit textMessage(query.lastError().text(), BaseViewer::Error);
    }
    else if (query.isSelect())
    {
        emit textMessage(tr("Sql query executed successfully"));
    }
    else
    {
        emit textMessage(tr("Query executed successfully, number affected rows: %1").arg(query.numRowsAffected()));
    }
}

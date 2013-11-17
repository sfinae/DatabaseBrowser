
#include <QTableView>
#include <QGroupBox>
#include <QVBoxLayout>

#include "TableView.h"


TableView::TableView(QWidget *parent, Qt::WindowFlags flags)
    : QWidget(parent, flags)
{
    box = new QGroupBox(this);
    table = new QTableView();

    QVBoxLayout *tableLayout = new QVBoxLayout;
    tableLayout->addWidget(table);
    box->setLayout(tableLayout);

    QVBoxLayout *boxLayout = new QVBoxLayout;
    boxLayout->addWidget(box);

    setLayout(boxLayout);
}

TableView::operator QTableView*() const
{
    return table;
}

QTableView* TableView::operator->()
{
    return table;
}

QTableView& TableView::operator*()
{
    return *table;
}

void TableView::setText(QString text)
{
    box->setTitle(text);
}

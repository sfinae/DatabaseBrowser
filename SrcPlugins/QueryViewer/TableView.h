#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <QWidget>

class QTableView;
class QGroupBox;

class TableView : public QWidget
{
	Q_OBJECT

public:
    TableView(QWidget *parent = 0, Qt::WindowFlags flags = 0);

	void setText(QString text);

	operator QTableView*() const;
	QTableView& operator*();
	QTableView* operator->();

private:
	QTableView *table;
	QGroupBox *box;
};

#endif // TABLE_VIEW_H

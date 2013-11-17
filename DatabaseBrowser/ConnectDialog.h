#ifndef CONNECT_DIALOG_H
#define CONNECT_DIALOG_H

#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QSettings>
#include <QSqlDatabase>

class ConnectDialog : public QDialog
{
	Q_OBJECT

public:
    ConnectDialog(int numberOfConnection = 0, QWidget *parent = 0, Qt::WindowFlags flags = 0);

    QSqlDatabase database() const;

private slots:
	void onConnect();
    void onDatabaseDriverChanged(int driverIndex);

protected:
	virtual void changeEvent(QEvent *event);

private:
	void retranslate();

    QSqlDatabase dataBase;

	QComboBox *comboDriver;
	QLineEdit *editHostName;
	QLineEdit *editDatabaseName;
	QLineEdit *editUserName;
	QLineEdit *editPassword;
	QLineEdit *editPortNumber;

	QLabel *labelDriver;
	QLabel *labelHostName;
	QLabel *labelDatabaseName;
	QLabel *labelUserName;
	QLabel *labelPassword;
	QLabel *labelPortNumber;

	QPushButton *buttonConnect;
	QPushButton *buttonMore;

	QSettings  *loginSettings;

	int connectionNumber;

};

#endif // CONNECT_DIALOG_H

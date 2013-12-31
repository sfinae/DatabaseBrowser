#ifndef CONNECT_DIALOG_H
#define CONNECT_DIALOG_H

#include <QDialog>
#include <QScopedPointer>
#include <QSqlDatabase>

class ConnectionDialogPrivate;

class ConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    ConnectionDialog(int numberOfConnection = 0,
                     QWidget *parent = 0,
                     Qt::WindowFlags flags = 0);
    ~ConnectionDialog();

    QSqlDatabase database() const;

private slots:
    void onConnect();
    void onDatabaseDriverChanged(int driverIndex);

protected:
    virtual void changeEvent(QEvent *event);

private:
    void retranslate();

    QScopedPointer<ConnectionDialogPrivate> d_ptr;

};

#endif // CONNECT_DIALOG_H

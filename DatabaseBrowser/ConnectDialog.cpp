
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QEvent>

#include "ConnectDialog.h"

ConnectDialog::ConnectDialog(int numberOfConnection, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , connectionNumber(numberOfConnection)
{
    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::Box);
    //frame->hide();

    labelDriver = new QLabel(this);
    comboDriver = new QComboBox(this);

    labelUserName = new QLabel(this);
    editUserName = new QLineEdit(this);

    labelPassword = new QLabel(this);
    editPassword = new QLineEdit(this);
    editPassword->setEchoMode(QLineEdit::Password);

    labelHostName = new QLabel(this);
    editHostName = new QLineEdit(this);

    labelDatabaseName = new QLabel(this);
    editDatabaseName = new QLineEdit(this);

    labelPortNumber = new QLabel(this);
    editPortNumber = new QLineEdit(this);
    editPortNumber->setInputMask("000000");

    buttonConnect = new QPushButton(this);
    buttonConnect->setDefault(true);

    //buttonMore = new QPushButton(this);
    //buttonMore->setCheckable(true);

    QGridLayout *frameLayout = new QGridLayout(frame);
    frameLayout->addWidget(labelUserName, 0, 0);
    frameLayout->addWidget(editUserName, 0, 1);
    frameLayout->addWidget(labelPassword, 1, 0);
    frameLayout->addWidget(editPassword, 1, 1);
    frameLayout->addWidget(labelHostName, 2, 0);
    frameLayout->addWidget(editHostName, 2, 1);
    frameLayout->addWidget(labelDatabaseName, 3, 0);
    frameLayout->addWidget(editDatabaseName, 3, 1);
    frameLayout->addWidget(labelPortNumber, 4, 0);
    frameLayout->addWidget(editPortNumber, 4, 1);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(buttonConnect);
    //buttonLayout->addStretch();
    //buttonLayout->addWidget(buttonMore);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(labelDriver, 0, 0);
    layout->addWidget(comboDriver, 0, 1);
    layout->addWidget(frame, 1, 0, 1, 3);
    layout->addLayout(buttonLayout, 0, 2);

    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    loginSettings = new QSettings("MichaelCompany", "DBMConverter", this);

    QString driver = loginSettings->value("Login/Driver").toString();
    if (!driver.isEmpty())
    {
        for (int i = 0; i < comboDriver->count(); ++i)
        {
            if (comboDriver->itemText(i) == driver)
            {
                comboDriver->setCurrentIndex(i);
                break;
            }
        }
    }

    editHostName->setText(loginSettings->value("Login/HostName").toString());
    editDatabaseName->setText(loginSettings->value("Login/DatabaseName").toString());
    editUserName->setText(loginSettings->value("Login/UserName").toString());
    editPassword->setText(QString::null);
    editPortNumber->setText(loginSettings->value("Login/PortNumber").toString());

    connect(buttonConnect, SIGNAL(clicked()), this, SLOT(onConnect()));
    //connect(buttonMore, SIGNAL(toggled(bool)), frame, SLOT(setVisible(bool)));
    connect(comboDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(onDatabaseDriverChanged(int)));

    QStringList listOfDatabase = QSqlDatabase::drivers();
    comboDriver->addItems(listOfDatabase);

    retranslate();
}

QSqlDatabase ConnectDialog::database() const
{
    Q_ASSERT(dataBase.isValid());
    return dataBase;
}

void ConnectDialog::onConnect()
{
    dataBase = QSqlDatabase::addDatabase(comboDriver->currentText(), QString("connect%1").arg(connectionNumber));

    dataBase.setHostName(editHostName->text());
    dataBase.setDatabaseName(editDatabaseName->text());
    dataBase.setUserName(editUserName->text());
    dataBase.setPassword(editPassword->text());
    dataBase.setPort(editPortNumber->text().toInt());

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool ok = dataBase.open();
    QApplication::restoreOverrideCursor();

    if (ok)
    {
        loginSettings->beginGroup("Login");
        loginSettings->setValue("HostName",		editHostName->text());
        loginSettings->setValue("DatabaseName", editDatabaseName->text());
        loginSettings->setValue("UserName",		editUserName->text());
        loginSettings->setValue("PortNumber",	editPortNumber->text());
        loginSettings->setValue("Driver",		comboDriver->currentText());
        loginSettings->endGroup();
        accept();
        return;
    }
    else
    {
        QSqlDatabase::removeDatabase(QString("connect%1").arg(connectionNumber));
        QMessageBox::critical(0, tr("Can\'t open database "), dataBase.lastError().text());
    }

    //reject();
}

void ConnectDialog::onDatabaseDriverChanged(int driverIndex)
{
    QString driver = comboDriver->itemText(driverIndex);

    if (driver == "QSQLITE")
    {
        labelHostName->hide();
        editHostName->hide();

        labelUserName->hide();
        editUserName->hide();

        labelPassword->hide();
        editPassword->hide();

        labelPortNumber->hide();
        editPortNumber->hide();
    }
    else
    {
        labelHostName->show();
        editHostName->show();

        labelUserName->show();
        editUserName->show();

        labelPassword->show();
        editPassword->show();

        labelPortNumber->show();
        editPortNumber->show();
    }
}

void ConnectDialog::retranslate()
{
    labelDriver->setText(tr("Database type"));
    labelUserName->setText(tr("User"));
    labelPassword->setText(tr("Password"));
    labelHostName->setText(tr("Host"));
    labelDatabaseName->setText(tr("Database"));
    labelPortNumber->setText(tr("Port"));
    buttonConnect->setText(tr("Connect"));
    //buttonMore->setText(tr("More"));
    setWindowTitle(tr("Connection do database"));
}

void ConnectDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        retranslate();
    }
}

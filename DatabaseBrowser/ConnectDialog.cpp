
#include "ConnectDialog.h"

#include <QSettings>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QSqlError>
#include <QEvent>
#include <QFileDialog>

class ConnectionDialogPrivate
{
public:
    QSqlDatabase m_database;

    QSettings *m_loginSettings;

    int m_connectionNumber;

    // ui
    QComboBox *m_comboDriver;
    QLineEdit *m_editHostName;
    QLineEdit *m_editDatabaseName;
    QLineEdit *m_editUserName;
    QLineEdit *m_editPassword;
    QLineEdit *m_editPortNumber;

    QLabel *m_labelDriver;
    QLabel *m_labelHostName;
    QLabel *m_labelDatabaseName;
    QLabel *m_labelUserName;
    QLabel *m_labelPassword;
    QLabel *m_labelPortNumber;

    QPushButton *m_buttonConnect;
    QPushButton *m_buttonMore;
    QPushButton *m_buttonChooseDb;
};

ConnectionDialog::ConnectionDialog(int numberOfConnection,
                                   QWidget *parent,
                                   Qt::WindowFlags flags)
    : QDialog(parent, flags)
    , d_ptr(new ConnectionDialogPrivate())
{
    d_ptr->m_connectionNumber = numberOfConnection;

    QFrame *frame = new QFrame(this);
    frame->setFrameShape(QFrame::Box);
    //frame->hide();

    d_ptr->m_labelDriver = new QLabel(this);
    d_ptr->m_comboDriver = new QComboBox(this);

    d_ptr->m_labelUserName = new QLabel(this);
    d_ptr->m_editUserName = new QLineEdit(this);

    d_ptr->m_labelPassword = new QLabel(this);
    d_ptr->m_editPassword = new QLineEdit(this);
    d_ptr->m_editPassword->setEchoMode(QLineEdit::Password);

    d_ptr->m_labelHostName = new QLabel(this);
    d_ptr->m_editHostName = new QLineEdit(this);

    d_ptr->m_labelDatabaseName = new QLabel(this);
    d_ptr->m_editDatabaseName = new QLineEdit(this);

    d_ptr->m_labelPortNumber = new QLabel(this);
    d_ptr->m_editPortNumber = new QLineEdit(this);
    d_ptr->m_editPortNumber->setInputMask("000000");

    d_ptr->m_buttonConnect = new QPushButton(this);
    d_ptr->m_buttonConnect->setDefault(true);

    d_ptr->m_buttonChooseDb = new QPushButton(this);
    d_ptr->m_buttonChooseDb->setText("...");

    QGridLayout *frameLayout = new QGridLayout(frame);
    frameLayout->addWidget(d_ptr->m_labelUserName, 0, 0);
    frameLayout->addWidget(d_ptr->m_editUserName, 0, 1);
    frameLayout->addWidget(d_ptr->m_labelPassword, 1, 0);
    frameLayout->addWidget(d_ptr->m_editPassword, 1, 1);
    frameLayout->addWidget(d_ptr->m_labelHostName, 2, 0);
    frameLayout->addWidget(d_ptr->m_editHostName, 2, 1);
    frameLayout->addWidget(d_ptr->m_labelDatabaseName, 3, 0);
    frameLayout->addWidget(d_ptr->m_editDatabaseName, 3, 1);
    frameLayout->addWidget(d_ptr->m_buttonChooseDb, 3, 2);
    frameLayout->addWidget(d_ptr->m_labelPortNumber, 4, 0);
    frameLayout->addWidget(d_ptr->m_editPortNumber, 4, 1);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->addWidget(d_ptr->m_buttonConnect);

    QGridLayout *layout = new QGridLayout(this);
    layout->addWidget(d_ptr->m_labelDriver, 0, 0);
    layout->addWidget(d_ptr->m_comboDriver, 0, 1);
    layout->addWidget(frame, 1, 0, 1, 3);
    layout->addLayout(buttonLayout, 0, 2);

    //layout->setSizeConstraint(QLayout::SetFixedSize);
    setLayout(layout);

    d_ptr->m_loginSettings = new QSettings("MichaelCompany", "DBMConverter", this);

    QString driver = d_ptr->m_loginSettings->value("Login/Driver").toString();
    if (!driver.isEmpty())
    {
        for (int i = 0; i < d_ptr->m_comboDriver->count(); ++i)
        {
            if (d_ptr->m_comboDriver->itemText(i) == driver)
            {
                d_ptr->m_comboDriver->setCurrentIndex(i);
                break;
            }
        }
    }

    d_ptr->m_editHostName->setText(d_ptr->m_loginSettings->value("Login/HostName").toString());
    d_ptr->m_editDatabaseName->setText(d_ptr->m_loginSettings->value("Login/DatabaseName").toString());
    d_ptr->m_buttonConnect->setDisabled(d_ptr->m_loginSettings->value("Login/DatabaseName").toString().isEmpty());
    d_ptr->m_editUserName->setText(d_ptr->m_loginSettings->value("Login/UserName").toString());
    d_ptr->m_editPassword->setText(QString::null);
    d_ptr->m_editPortNumber->setText(d_ptr->m_loginSettings->value("Login/PortNumber").toString());

    connect(d_ptr->m_buttonConnect, SIGNAL(clicked()), this, SLOT(onConnect()));
    connect(d_ptr->m_comboDriver, SIGNAL(currentIndexChanged(int)), this, SLOT(onDatabaseDriverChanged(int)));
    connect(d_ptr->m_buttonChooseDb, SIGNAL(clicked()), this, SLOT(chooseDb()));

    QStringList listOfDatabase = QSqlDatabase::drivers();
    d_ptr->m_comboDriver->addItems(listOfDatabase);

    retranslate();
}

ConnectionDialog::~ConnectionDialog()
{
    // empty
}

QSqlDatabase ConnectionDialog::database() const
{
    Q_ASSERT(d_ptr->m_database.isValid());
    return d_ptr->m_database;
}

void ConnectionDialog::onConnect()
{
    d_ptr->m_database = QSqlDatabase::addDatabase(d_ptr->m_comboDriver->currentText(),
                                                  QString("connect%1").arg(d_ptr->m_connectionNumber));

    d_ptr->m_database.setHostName(d_ptr->m_editHostName->text());
    d_ptr->m_database.setDatabaseName(d_ptr->m_editDatabaseName->text());
    d_ptr->m_database.setUserName(d_ptr->m_editUserName->text());
    d_ptr->m_database.setPassword(d_ptr->m_editPassword->text());
    d_ptr->m_database.setPort(d_ptr->m_editPortNumber->text().toInt());

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    bool ok = d_ptr->m_database.open();
    QApplication::restoreOverrideCursor();

    if (ok)
    {
        d_ptr->m_loginSettings->beginGroup("Login");
        d_ptr->m_loginSettings->setValue("HostName", d_ptr->m_editHostName->text());
        d_ptr->m_loginSettings->setValue("DatabaseName", d_ptr->m_editDatabaseName->text());
        d_ptr->m_loginSettings->setValue("UserName", d_ptr->m_editUserName->text());
        d_ptr->m_loginSettings->setValue("PortNumber", d_ptr->m_editPortNumber->text());
        d_ptr->m_loginSettings->setValue("Driver", d_ptr->m_comboDriver->currentText());
        d_ptr->m_loginSettings->endGroup();

        accept();
    }
    else
    {
        QSqlDatabase::removeDatabase(QString("connect%1").arg(d_ptr->m_connectionNumber));
        QMessageBox::critical(0, tr("Can\'t open database "), d_ptr->m_database.lastError().text());
    }
}

void ConnectionDialog::onDatabaseDriverChanged(int driverIndex)
{
    QString driver = d_ptr->m_comboDriver->itemText(driverIndex);

    if (driver == "QSQLITE")
    {
        d_ptr->m_labelHostName->hide();
        d_ptr->m_editHostName->hide();

        d_ptr->m_labelUserName->hide();
        d_ptr->m_editUserName->hide();

        d_ptr->m_labelPassword->hide();
        d_ptr->m_editPassword->hide();

        d_ptr->m_labelPortNumber->hide();
        d_ptr->m_editPortNumber->hide();
    }
    else
    {
        d_ptr->m_labelHostName->show();
        d_ptr->m_editHostName->show();

        d_ptr->m_labelUserName->show();
        d_ptr->m_editUserName->show();

        d_ptr->m_labelPassword->show();
        d_ptr->m_editPassword->show();

        d_ptr->m_labelPortNumber->show();
        d_ptr->m_editPortNumber->show();
    }
}

void ConnectionDialog::chooseDb()
{
    QFileDialog fd(this, "Choose DB");
    fd.setFilter(QDir::AllEntries | QDir::Hidden | QDir::NoSymLinks);
    fd.setDirectory(QDir::homePath());
    fd.exec();
    QStringList selectedFiles = fd.selectedFiles();
    if (selectedFiles.isEmpty())
    {
        return;
    }

    d_ptr->m_editDatabaseName->setText(selectedFiles.first());
    d_ptr->m_buttonConnect->setDisabled(selectedFiles.isEmpty());
}

void ConnectionDialog::retranslate()
{
    d_ptr->m_labelDriver->setText(tr("Database type"));
    d_ptr->m_labelUserName->setText(tr("User"));
    d_ptr->m_labelPassword->setText(tr("Password"));
    d_ptr->m_labelHostName->setText(tr("Host"));
    d_ptr->m_labelDatabaseName->setText(tr("Database"));
    d_ptr->m_labelPortNumber->setText(tr("Port"));
    d_ptr->m_buttonConnect->setText(tr("Connect"));

    setWindowTitle(tr("Connection do database"));
}

void ConnectionDialog::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
    {
        retranslate();
    }
}

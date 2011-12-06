#include <QtCore/QStringList>
#include <QtCore/QTimer>
//#include <QtCore/QDebug>

#include "maindialog.h"
#include "ui_maindialog.h"
#include "optionsdialog.h"
#include "tracedialog.h"

#include "serialportinfo.h"
#include "serialport.h"


/* Public methods */


MainDialog::MainDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MainDialog)
    , m_port(0)
    , m_timer(0)
    , m_rts(false)
    , m_dtr(false)
{
    ui->setupUi(this);
    fillOpenModeComboBox();

    m_port = new SerialPort(this);
    m_timer = new QTimer(this);
    m_timer->setInterval(500);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(procUpdateLines()));

    procShowPorts();
    int idx = ui->boxName->currentIndex();
    if (idx >= 0)
        procItemPortChanged(idx);

    connect(ui->boxName, SIGNAL(currentIndexChanged(int)), this, SLOT(procItemPortChanged(int)));
    connect(ui->controlButton, SIGNAL(clicked()), this, SLOT(procControlButtonClick()));
    connect(ui->optionsButton, SIGNAL(clicked()), this, SLOT(procOptionsButtonClick()));
    connect(ui->ioButton, SIGNAL(clicked()), this, SLOT(procIOButtonClick()));
    connect(ui->rtsButton, SIGNAL(clicked()), this, SLOT(procRtsButtonClick()));
    connect(ui->dtrButton, SIGNAL(clicked()), this, SLOT(procDtrButtonClick()));
}

MainDialog::~MainDialog()
{
    if (m_port->isOpen())
        m_port->close();
    delete ui;
}


/* Protected methods */


void MainDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


/* Private slots */


void MainDialog::procShowPorts()
{
    ui->boxName->clear();
    foreach(SerialPortInfo inf, SerialPortInfo::availablePorts()) {
        QStringList sl;
        sl << inf.systemLocation() << inf.description() << inf.manufacturer();
        ui->boxName->addItem(inf.portName(), QVariant(sl));
    }
}

void MainDialog::procItemPortChanged(int idx)
{
    QStringList sl = ui->boxName->itemData(idx).toStringList();
    ui->lbLocation->setText(sl.at(0));
    ui->lbDescr->setText(sl.at(1));
    ui->lbMfg->setText(sl.at(2));
}

void MainDialog::procControlButtonClick()
{
    if (m_port->isOpen()) {
        m_timer->stop();
        m_port->close();
        ui->controlButton->setText(tr("Open"));
        ui->optionsButton->setEnabled(false);
        ui->ioButton->setEnabled(false);
        ui->rtsButton->setEnabled(false);
        ui->dtrButton->setEnabled(false);
        ui->boxName->setEnabled(true);
        ui->modeGroupBox->setEnabled(true);
    } else {
        m_port->setPort(ui->boxName->currentText());
        int idx = ui->modeComboBox->currentIndex();
        bool ok = false;
        idx = ui->modeComboBox->itemData(idx).toInt(&ok);
        if (ok && m_port->open((QIODevice::OpenMode)idx)) {
            ui->controlButton->setText(tr("Close"));
            ui->optionsButton->setEnabled(true);
            ui->ioButton->setEnabled(true);
            ui->rtsButton->setEnabled(true);
            ui->dtrButton->setEnabled(true);
            ui->boxName->setEnabled(false);
            ui->modeGroupBox->setEnabled(false);
            m_timer->start();
        }
    }
}

void MainDialog::procOptionsButtonClick()
{
    OptionsDialog dlg(m_port);
    dlg.exec();
}

void MainDialog::procIOButtonClick()
{
    TraceDialog dlg(m_port);
    dlg.exec();
}

void MainDialog::procRtsButtonClick()
{
    m_port->setRts(!m_rts);
}

void MainDialog::procDtrButtonClick()
{
    m_port->setDtr(!m_dtr);
}

void MainDialog::procUpdateLines()
{
    SerialPort::Lines lines = m_port->lines();
    m_rts = SerialPort::Rts & lines;
    m_dtr = SerialPort::Dtr & lines;

    ui->leLabel->setEnabled(SerialPort::Le & lines);
    ui->dtrLabel->setEnabled(m_dtr);
    ui->rtsLabel->setEnabled(m_rts);
    //ui->stLabel->setEnabled(SerialPort::St & lines);
    //ui->srLabel->setEnabled(SerialPort::Sr & lines);
    ui->ctsLabel->setEnabled(SerialPort::Cts & lines);
    ui->dcdLabel->setEnabled(SerialPort::Dcd & lines);
    ui->ringLabel->setEnabled(SerialPort::Ri & lines);
    ui->dsrLabel->setEnabled(SerialPort::Dsr & lines);

    ui->rtsButton->setText((m_rts) ? tr("Clear RTS") : tr("Set RTS"));
    ui->dtrButton->setText((m_dtr) ? tr("Clear DTR") : tr("Set DTR"));
}


/* Private methods */


void MainDialog::fillOpenModeComboBox()
{
    ui->modeComboBox->addItem(QString(tr("Read and write")), QVariant(QIODevice::ReadWrite));
    ui->modeComboBox->addItem(QString(tr("Read only")), QVariant(QIODevice::ReadOnly));
    ui->modeComboBox->addItem(QString(tr("Write only")), QVariant(QIODevice::WriteOnly));
}
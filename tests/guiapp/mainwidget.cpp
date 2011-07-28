#include <QtCore/QStringList>
//#include <QtCore/QDebug>

#include "mainwidget.h"
#include "ui_mainwidget.h"
#include "optionswidget.h"
#include "tracewidget.h"

#include "serialportinfo.h"
#include "serialport.h"


/* Public methods */


MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MainWidget)
    , m_optionsWidget(0)
    , m_traceWidget(0)
    , m_port(0)
{
    ui->setupUi(this);
    m_port = new SerialPort(this);

    procShowPorts();
    procItemPortChanged(ui->boxName->currentIndex());

    connect(ui->boxName, SIGNAL(currentIndexChanged(int)), this, SLOT(procItemPortChanged(int)));
    connect(ui->controlButton, SIGNAL(clicked()), this, SLOT(procControlButtonClick()));
    connect(ui->optionsButton, SIGNAL(clicked()), this, SLOT(procOptionsButtonClick()));
    connect(ui->ioButton, SIGNAL(clicked()), this, SLOT(procIOButtonClick()));
}

MainWidget::~MainWidget()
{
    if (m_port->isOpen())
        m_port->close();
    if (m_optionsWidget)
        delete m_optionsWidget;
    if (m_traceWidget)
        delete m_traceWidget;
    delete ui;
}


/* Protected methods */


void MainWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}


/* Private slots */


void MainWidget::procShowPorts()
{
    ui->boxName->clear();
    foreach(SerialPortInfo inf, SerialPortInfo::availablePorts()) {
        QStringList sl;
        sl << inf.systemLocation() << inf.description() << inf.manufacturer();
        ui->boxName->addItem(inf.portName(), QVariant(sl));
    }
}

void MainWidget::procItemPortChanged(int idx)
{
    QStringList sl = ui->boxName->itemData(idx).toStringList();
    ui->lbLocation->setText(sl.at(0));
    ui->lbDescr->setText(sl.at(1));
    ui->lbMfg->setText(sl.at(2));
}

void MainWidget::procControlButtonClick()
{
    if (m_port->isOpen()) {
        m_port->close();
        ui->controlButton->setText(tr("Open"));
        ui->optionsButton->setEnabled(false);
        ui->ioButton->setEnabled(false);
        ui->rtsButton->setEnabled(false);
        ui->dtrButton->setEnabled(false);
        ui->boxName->setEnabled(true);
    }
    else {
        m_port->setPort(ui->boxName->currentText());
        if (m_port->open(QIODevice::ReadWrite)) {
            ui->controlButton->setText(tr("Close"));
            ui->optionsButton->setEnabled(true);
            ui->ioButton->setEnabled(true);
            ui->rtsButton->setEnabled(true);
            ui->dtrButton->setEnabled(true);
            ui->boxName->setEnabled(false);
        }
    }
}

void MainWidget::procOptionsButtonClick()
{
    if (!m_optionsWidget)
        m_optionsWidget = new OptionsWidget(m_port);
    m_optionsWidget->show();
}

void MainWidget::procIOButtonClick()
{
    if (!m_traceWidget)
        m_traceWidget = new TraceWidget(m_port);
    m_traceWidget->show();
}















#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    openSerialPort();
}

Widget::~Widget()
{
    qDebug() << __LINE__ << ":~Widget\r\n";
    delete wPortList;
    delete ui;
}


/*************************************** Serial Port Operation [Begin] ******************************************/

void Widget::onNewPortList(QStringList portName)
{
    qDebug() << portName;
}

void Widget::openSerialPort()
{
    wPortList = new SerialPortList();
    connect(wPortList, SIGNAL(onNewSerialPort(QStringList)), this, SLOT(onNewPortList(QStringList)));
    wPortList->ScanStart();
}

void Widget::closeSerialPort()
{
    wPortList->ScanStop();
}



/*************************************** Serial Port Operation [end] ******************************************/

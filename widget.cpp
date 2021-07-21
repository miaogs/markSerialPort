#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*Forms without maxinize and minimize buttons, and title bar*/
    this->setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint);
    /*Translucent, transparency is 0.8*/
    this->setWindowOpacity(0.8);



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

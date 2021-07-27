#include <QDebug>

//Customized
#include "serialportlist.h"

// Default constructor, executed each time a new class object is created.
//SerialPortList::SerialPortList(QObject *parent) : QObject(parent)
SerialPortList::SerialPortList()
{
    timer = new QTimer();
    scanCycleMs = 200;
    newPortStringList.clear();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
}


// Default constructor, executed each time a new class object is created.
SerialPortList::SerialPortList(quint16 CycleMs)
{
    timer = new QTimer();
    scanCycleMs = CycleMs;
    newPortStringList.clear();
    connect(timer, SIGNAL(timeout()), this, SLOT(onTimeOut()));
}

SerialPortList::~SerialPortList()
{
    qDebug() << __LINE__ << ":~SerialPortList\r\n";
    timer->stop();
    delete timer;
}


// Start Scanning
void SerialPortList::ScanStart()
{
    timer->stop();
    timer->start(scanCycleMs);
}


// Stop Scanning
void SerialPortList::ScanStop()
{
    timer->stop();
}


// Cycle Sacn Serivice
void SerialPortList::onTimeOut()
{
    QStringList tempPortStringList;
    /*Start Scanning*/
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
//        qDebug() << "Name             :" << info.portName();
//        qDebug() << "Description      :" << info.description();
//        qDebug() << "Manufacturer     :" << info.manufacturer();
        tempPortStringList += info.portName();
    }

    /*Update the List*/
    if(newPortStringList.size() != tempPortStringList.size())
    {
        newPortStringList = tempPortStringList;
        emit onNewSerialPort(newPortStringList);/*Customized signal*/
    }

}

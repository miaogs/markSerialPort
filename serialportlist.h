#ifndef SERIALPORTLIST_H
#define SERIALPORTLIST_H

#include <QObject>
#include <QTimer>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>

class SerialPortList : public QObject
{
    Q_OBJECT

public:
//    explicit SerialPortList(QObject *parent = nullptr);
    explicit SerialPortList();/*This is the constructor without custom parameters*/
    explicit SerialPortList(quint16 CycleMs);/*This is the constructor with custom parameters*/
    ~SerialPortList();

    quint16 scanCycleMs;
    QTimer *timer;
    QStringList newPortStringList;


    void ScanStart();
    void ScanStop();

public slots:
    void  onTimeOut();

signals:
    void onNewSerialPort(QStringList);

};

#endif // SERIALPORTLIST_H

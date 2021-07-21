#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

//Customer defined
#include "serialportlist.h"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //Customer defined
    SerialPortList *wPortList;

    void openSerialPort();
    void closeSerialPort();

private slots:
    void onNewPortList(QStringList portName);

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>

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


signals:
    void onLabeltoLineText(QString strText);

public slots:
    /*Overwrite*/
    bool eventFilter(QObject* obj, QEvent* evt);
    void onUpdateLabel(QString strText);

private slots:
    void onNewPortList(QStringList portName);

private:
    Ui::Widget *ui;

protected:
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

    bool m_bIsWindowMoveable;
};
#endif // WIDGET_H

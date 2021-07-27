#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QMenu>

//Customized
#include "serialportlist.h"

#define COMBOBOX_DEFAULT_TEXT  ""
#define LABEL_DEFAULT_TEXT  "Select"

#define LABEL_NEW_TEXT  "Add Label"


QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    //Customized
    QMenu *mMenu;
    QAction *mSettingAction;
    QAction *mExitAppAction;
    QAction *mAboutAppAction;
    QSystemTrayIcon* mSysTrayIcon;
    SerialPortList *wPortList;
    QString currentSerialPortText;
    QString currentSerialPortLastText;
    int insertComboBoxIndex;
    int insertComboBoxLastIndex;

    struct ShowSettings
        {
            QString serialPort;
            QString labelText;
            bool lockFlag; /*lock :true  ,not lock :false*/

            ShowSettings(QString s = COMBOBOX_DEFAULT_TEXT, QString l = COMBOBOX_DEFAULT_TEXT, bool f = false)//利用构造函数初始化
            {
                serialPort = s;
                labelText = l;
                lockFlag = f;
            }
        };

    struct Settings
    {
        ShowSettings showsettings[2];
    };

    Settings settings() const;

    void openSerialPort();
    void closeSerialPort();
    void createMenu();
    void createActions();

signals:
    void onLabeltoLineText(QString strText);

public slots:
    /*Overwrite*/
    bool eventFilter(QObject* obj, QEvent* evt);
    void onUpdateLabel(QString strText);
    void onSettingAction();
    void onExitAppAction();
    void onAboutAppAction();

private slots:
    void onNewPortList(QStringList portName);
    void onChooseSerialPort(int index);
    void onChooseSerialPortLast(int index);
    void onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::Widget *ui;

    //Customized
    Settings currentSettings;

    void readSettings();
    void updateSettings();

protected:
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

    bool m_bIsWindowMoveable;
};
#endif // WIDGET_H

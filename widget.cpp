#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>
#include <QMessageBox>
#include <QSettings>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*title*/
    this->setWindowTitle(QString::fromLocal8Bit("MarkSP"));

    /*Window Icon*/
    this->setWindowIcon(QIcon(":/logo/images/icon.ico"));

    /*Forms without maxinize and minimize buttons, and title bar*/
    this->setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint);
    /*Translucent, transparency is 0.8*/
    this->setWindowOpacity(0.8);

    /*Install QEvent Filters*/
    ui->label->installEventFilter(this);
    ui->lineEdit->installEventFilter(this);
    ui->labelLast->installEventFilter(this);
    ui->lineEditLast->installEventFilter(this);

//    ui->comboBoxList->setStyleSheet("QComboBox{color:black; background-color:red}"
//                                    "QComboBox::drop-arrow{background:rgb(0,245,255);}"
//                                    "QComboBox::pressed{background:#696969;}");/*more refer to : https://www.sojson.com/rgb.html and https://blog.csdn.net/xiaopei_yan/article/details/107404698*/

//    ui->label->setStyleSheet("background:green");

    openSerialPort();

    //debug begin
    QSettings iniwrite("config.ini",QSettings::IniFormat);
    iniwrite.beginGroup("Mark_0");
    iniwrite.setValue("labelText","COM11 Label");
    iniwrite.setValue("serialPort","COM11");
    iniwrite.endGroup();
    iniwrite.beginGroup("Mark 1");
    iniwrite.setValue("labelText","COM12 Label");
    iniwrite.setValue("serialPort","COM12");
    iniwrite.endGroup();
    //debug end

    readSettings();

    connect(ui->comboBoxList, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onChooseSerialPort(int)));//当前ComboBox中的CurrentIndex改变时，不管是交互式还是通过程序改变选项，都会产生一个改变值得消息信号，如果ComboBox为空或重置当前ComboBox，产生的消息值返回-1

    connect(ui->comboBoxListLast, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onChooseSerialPortLast(int)));

    /*Mouse double click to modify text*/
    connect(this, SIGNAL(onLabeltoLineText(QString)),
            this, SLOT(onUpdateLabel(QString)));

    mSysTrayIcon = new QSystemTrayIcon(this);
    QIcon icon = QIcon(":/logo/images/icon.ico");
    mSysTrayIcon->setIcon(icon);

    /*when the mouse moves to the icon in the tary, the content set here is displayed*/
    mSysTrayIcon->setToolTip(QObject::trUtf8("MarkSP"));

    /*add a SLOT function to mSysTrayIcon*/
    connect(mSysTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason)));

    /*Create menus for tray operations*/
    createActions();
    createMenu();

    /*Show mSysTrayIcon in the system tray*/
    mSysTrayIcon->show();

}

Widget::~Widget()
{
    qDebug() << __LINE__ << ":~Widget\r\n";
    closeSerialPort();
    delete wPortList;
    delete ui;
    delete mSysTrayIcon;
    delete mMenu;
    delete mSettingAction;
}


/*************************************** Settings [Begin] *****************************************************/
Widget::Settings Widget::settings() const
{
    return currentSettings;
}


void Widget::readSettings()
{
    //读取配置文件
    QSettings *iniRead = new QSettings("config.ini", QSettings::IniFormat);
    if(iniRead)
    {
        qDebug() << "Found config.ini file";
        iniRead->beginGroup("Mark_0");
        currentSettings.showsettings[0].serialPort = iniRead->value("serialPort").toString();
        currentSettings.showsettings[0].labelText = iniRead->value("labelText").toString();
        qDebug() << currentSettings.showsettings[0].serialPort << ":" << currentSettings.showsettings[0].labelText;
        iniRead->endGroup();

        iniRead->beginGroup("Mark 1");
        currentSettings.showsettings[1].serialPort = iniRead->value("serialPort").toString();
        currentSettings.showsettings[1].labelText = iniRead->value("labelText").toString();
        qDebug() << currentSettings.showsettings[1].serialPort << ":" << currentSettings.showsettings[1].labelText;
        iniRead->endGroup();
    }else
    {
         qDebug() << "Not found config.ini file";
    }

    delete iniRead;
}


void Widget::updateSettings()
{
    QSettings *iniWrite = new QSettings("config.ini", QSettings::IniFormat);

    iniWrite->beginGroup("Mark_0");
    iniWrite->setValue("serialPort",currentSettings.showsettings[0].serialPort);
    iniWrite->setValue("labelText",currentSettings.showsettings[0].labelText);
    iniWrite->endGroup();

    iniWrite->beginGroup("Mark 1");
    iniWrite->setValue("serialPort",currentSettings.showsettings[1].serialPort);
    iniWrite->setValue("labelText",currentSettings.showsettings[1].labelText);
    iniWrite->endGroup();

    delete iniWrite;
}

/*************************************** Settings [End] *******************************************************/

/*************************************** System [Begin] *******************************************************/

//SystemTrayIcon
void Widget::onActivatedSysTrayIcon(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:/*trigger the tray icon*/
        break;
    case QSystemTrayIcon::DoubleClick:/*double click the tray icon*/
        this->show();/*double click to display the main program window*/
        break;
    default:
        break;

    }
}

/*************************************** System [End] **********************************************************/


/*************************************** Action [Begin] ********************************************************/

//SLOT
void Widget::onSettingAction()
{

}

void Widget::onAboutAppAction()
{

}

void Widget::onExitAppAction()
{
    exit(0);
    closeSerialPort();
    delete wPortList;
    delete ui;
    delete mSysTrayIcon;
    delete mMenu;
    delete mSettingAction;

}


void Widget::createActions()
{
    mSettingAction = new QAction(QObject::trUtf8("Setting"), this);
    connect(mSettingAction, SIGNAL(triggered()), this, SLOT(onSettingAction()));

    mAboutAppAction = new QAction(QObject::trUtf8("About"), this);
    connect(mAboutAppAction, SIGNAL(triggered()), this, SLOT(onAboutAppAction()));

    mExitAppAction = new QAction(QObject::trUtf8("Exit"), this);
    connect(mExitAppAction, SIGNAL(triggered()), this, SLOT(onExitAppAction()));

}

/*************************************** Action [End] **********************************************************/

/*************************************** Menu [Begin] **********************************************************/

void Widget::createMenu()
{
    mMenu = new QMenu(this);

    /*New menu item*/

    mMenu->addAction(mSettingAction);/*Setting interface*/
    mMenu->addSeparator();/*Add separator*/

    mMenu->addAction(mAboutAppAction);/*About application*/
    mMenu->addSeparator();/*Add separator*/

    mMenu->addAction(mExitAppAction);/*Exit application*/
    mMenu->addSeparator();/*Add separator*/

    /*Assign mMenu to mSysTrayIcon Object*/
    mSysTrayIcon->setContextMenu(mMenu);
}

/*************************************** Menu [End] ************************************************************/

/*************************************** ComboxBox Operation [Begin] *******************************************/
//SLOT
void Widget::onChooseSerialPort(int index)
{
    //TODO: check index parameter
    qDebug() << __LINE__ << ":" << " SerialPort Drop down selection: " << index;

    if(ui->comboBoxList->itemText(index) == COMBOBOX_DEFAULT_TEXT)//default
    {
        ui->stackedWidget->setCurrentIndex(0);

    }
    else
    {
        int var = 0;
        for (var = 0; var < ui->comboBoxListLast->count(); ++var) {
            if(ui->comboBoxListLast->itemText(var) == ui->comboBoxList->itemText(index))
            {
                currentSerialPortText = ui->comboBoxList->itemText(index);
                insertComboBoxLastIndex = var;
                ui->comboBoxListLast->blockSignals(true);
                ui->comboBoxListLast->removeItem(var);
                ui->comboBoxListLast->blockSignals(false);
                break;
            }
        }

        for (var = 0; var < 2; ++var) {
            if(currentSettings.showsettings[var].serialPort == ui->comboBoxList->itemText(index))/*update Label*/
            {
                ui->label->setText(currentSettings.showsettings[var].labelText);
                ui->stackedWidget->setCurrentIndex(0);
                qDebug() << __LINE__ << ":" << "Label Serial Port: " << currentSettings.showsettings[var].labelText;
                break;
            }
        }

        if(var == 2)
        {
            qDebug() << __LINE__ << ":" << "Add Default Label";

            // new serial port
            ui->label->setText(LABEL_NEW_TEXT);
            ui->stackedWidget->setCurrentIndex(0);
        }
    }

    qDebug() << __LINE__ << ":" << "currentSerialPortText:" << currentSerialPortText;
}


void Widget::onChooseSerialPortLast(int index)
{
    //TODO: check index parameter
    qDebug() << __LINE__ << ":" << "SerialPortLast Drop down selection:" << index;

    if(ui->comboBoxListLast->itemText(index) == COMBOBOX_DEFAULT_TEXT)//default
    {
        ui->stackedWidgetLast->setCurrentIndex(0);
    }
    else
    {
        int var = 0;
        for (var = 0; var < ui->comboBoxList->count(); ++var) {
            if(ui->comboBoxList->itemText(var) == ui->comboBoxListLast->itemText(index))// comboBoxListLast 选中COMX ，就要从 comboBoxList中 remove COMX ，否则会出现COMX显示重复
            {
                currentSerialPortLastText = ui->comboBoxListLast->itemText(index);
                insertComboBoxIndex = var;
                ui->comboBoxList->blockSignals(true);
                ui->comboBoxList->removeItem(var);
                ui->comboBoxList->blockSignals(false);
                break;
            }
        }

        for (var = 0; var < 2; ++var) {
            if(currentSettings.showsettings[var].serialPort == ui->comboBoxListLast->itemText(index))/*update Label*/
            {
                ui->labelLast->setText(currentSettings.showsettings[var].labelText);
                ui->stackedWidgetLast->setCurrentIndex(0);
                qDebug() << __LINE__ << ":" << "Label Serial Port: " << currentSettings.showsettings[var].labelText;
                break;
            }
        }

        if(var == 2)
        {
            qDebug() << __LINE__ << ":" << "Add Default Label";

            // new serial port
            ui->labelLast->setText(LABEL_NEW_TEXT);
            ui->stackedWidgetLast->setCurrentIndex(0);
        }
    }

    qDebug() << __LINE__ << ":" << "currentSerialPortLastText:" << currentSerialPortLastText;
}

/*************************************** ComboxBox Operation [End] *********************************************/


/*************************************** Mouse Event [Begin] ***************************************************/
//SLOT
void Widget::onUpdateLabel(QString strText)
{
    int var = 0;

    if(strText == "stackedWidget0")
    {
        if(ui->comboBoxList->currentText() != "")
        {
            ui->lineEdit->setText(ui->label->text());
            ui->stackedWidget->setCurrentIndex(1);
        }else
        {
            ui->label->setText("Select");
            ui->stackedWidget->setCurrentIndex(0);
        }

    }else if(strText == "stackedWidget1"){
        for (var = 0; var < 2; ++var) {
            if(currentSettings.showsettings[var].serialPort == ui->comboBoxList->currentText())/*Existing COM,update Label*/
            {
                currentSettings.showsettings[var].labelText = ui->lineEdit->text();
                currentSettings.showsettings[var].lockFlag = true;
                ui->label->setText(ui->lineEdit->text());
                ui->stackedWidget->setCurrentIndex(0);
                qDebug() << __LINE__ << ":" << "Update Label: " << currentSettings.showsettings[var].labelText;
                break;
            }
        }

        if(var == 2 )
        {
            for (var = 0; var < 2; ++var) {
                if(currentSettings.showsettings[var].serialPort == "")/*New COM,update Label*/
                {
                    currentSettings.showsettings[var].serialPort = ui->comboBoxList->currentText();
                    currentSettings.showsettings[var].labelText = ui->lineEdit->text();
                    currentSettings.showsettings[var].lockFlag = true;
                    ui->label->setText(ui->lineEdit->text());
                    ui->stackedWidget->setCurrentIndex(0);
                    break;
                }
            }

            if(var == 2)/*Replace the existing COM,then update Label*/
            {
                currentSettings.showsettings[0].serialPort = ui->comboBoxList->currentText();
                currentSettings.showsettings[0].labelText = ui->lineEdit->text();
                currentSettings.showsettings[0].lockFlag = true;
                ui->label->setText(ui->lineEdit->text());
                ui->stackedWidget->setCurrentIndex(0);
            }
            var = 0;
            qDebug() << __LINE__ << ":" << "Add New Serialport with Label";
        }


        /*The Settings will only updated if the Label is updated*/
        updateSettings();
    }else if(strText == "stackedWidgetLast0")
    {
        if(ui->comboBoxListLast->currentText() != "")
        {
            ui->lineEditLast->setText(ui->labelLast->text());
            ui->stackedWidgetLast->setCurrentIndex(1);
        }else
        {
            ui->labelLast->setText("Select");
            ui->stackedWidgetLast->setCurrentIndex(0);
        }

    }else if(strText == "stackedWidgetLast1"){
        for (var = 0; var < 2; ++var) {
            if(currentSettings.showsettings[var].serialPort == ui->comboBoxListLast->currentText())/*Existing COM,update Label*/
            {
                currentSettings.showsettings[var].labelText = ui->lineEditLast->text();
                currentSettings.showsettings[var].lockFlag = true;
                ui->labelLast->setText(ui->lineEditLast->text());
                ui->stackedWidgetLast->setCurrentIndex(0);
                qDebug() << __LINE__ << ":" << "Update Label: " << currentSettings.showsettings[var].labelText;
                break;
            }
        }

        if(var == 2 )
        {
            for (var = 0; var < 2; ++var) {
                if(currentSettings.showsettings[var].serialPort == "")/*New COM,update Label*/
                {
                    currentSettings.showsettings[var].serialPort = ui->comboBoxListLast->currentText();
                    currentSettings.showsettings[var].labelText = ui->lineEditLast->text();
                    currentSettings.showsettings[var].lockFlag = true;
                    ui->labelLast->setText(ui->lineEditLast->text());
                    ui->stackedWidgetLast->setCurrentIndex(0);
                    break;
                }
            }

            if(var == 2)/*Replace the existing COM,then update Label*/
            {
                currentSettings.showsettings[0].serialPort = ui->comboBoxListLast->currentText();
                currentSettings.showsettings[0].labelText = ui->lineEditLast->text();
                currentSettings.showsettings[0].lockFlag = true;
                ui->labelLast->setText(ui->lineEditLast->text());
                ui->stackedWidgetLast->setCurrentIndex(0);
            }
            var = 0;
            qDebug() << __LINE__ << ":" << "Add New Serialport with Label";
        }


        /*The Settings will only updated if the Label is updated*/
        updateSettings();
    }
}

//Overwrite
void Widget::mousePressEvent(QMouseEvent *e)
{
    if(e->button() == Qt::LeftButton)
    {
        m_bIsWindowMoveable = true;
    }
}

//Overwrite
void Widget::mouseMoveEvent(QMouseEvent *e)
{
    if(m_bIsWindowMoveable)
    {
        move(e->globalPos());
    }
}

//Overwrite
void Widget::mouseReleaseEvent(QMouseEvent *e)
{
    m_bIsWindowMoveable = false;
}

//Overwrite
void Widget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        this->showMinimized();/*minimize*/
    }else if (event->button() == Qt::RightButton) {
        QMessageBox::StandardButton result = QMessageBox::question(this, "Notice", "Are you sure you want to exit this application ?",
                        QMessageBox::Yes| QMessageBox::No |QMessageBox::Cancel,
                        QMessageBox:: No);
        if(result == QMessageBox::Yes)
        {
            this->close();/*Close*/
        }else if (result == QMessageBox::No) {
            //TODO
            this->hide();/*Hide*/
        }
    }
}


bool Widget::eventFilter(QObject* obj, QEvent* evt)
{

    if(obj == ui->label)
    {
        if(evt->type() == QEvent::MouseButtonDblClick)
        {
            /*Cumstomized Signals*/
            emit onLabeltoLineText("stackedWidget0");
            return true;
        }
    }else if(obj == ui->labelLast)
    {
        if(evt->type() == QEvent::MouseButtonDblClick)
        {
            /*Cumstomized Signals*/
            emit onLabeltoLineText("stackedWidgetLast0");
            return true;
        }
    }else if (obj == ui->lineEdit) {
        QKeyEvent *keyevt = static_cast<QKeyEvent*>(evt);

        if( ((evt->type() == QEvent::KeyPress) && ( (keyevt->key() == Qt::Key_Return) /*Enter key for large keyboard*/||
                                                    (keyevt->key() == Qt::Key_Escape) /*ESC*/||
                                                    (keyevt->key() == Qt::Key_Enter) /*Enter key for small keyboard*/))/*Key Event*/
                || (evt->type() == QEvent::FocusOut)/*FocusOut*/ )
        {
            /*Cumstomized Signals*/
            emit onLabeltoLineText("stackedWidget1");
            return true;
        }
    }else if (obj == ui->lineEditLast) {
        QKeyEvent *keyevt = static_cast<QKeyEvent*>(evt);

        if( ((evt->type() == QEvent::KeyPress) && ( (keyevt->key() == Qt::Key_Return) /*Enter key for large keyboard*/||
                                                    (keyevt->key() == Qt::Key_Escape) /*ESC*/||
                                                    (keyevt->key() == Qt::Key_Enter) /*Enter key for small keyboard*/))/*Key Event*/
                || (evt->type() == QEvent::FocusOut)/*FocusOut*/ )
        {
            /*Cumstomized Signals*/
            emit onLabeltoLineText("stackedWidgetLast1");
            return true;
        }
    }

    return QWidget::eventFilter(obj, evt);
}
/*************************************** Mouse Event [End] *****************************************************/


/*************************************** Serial Port Operation [Begin] *****************************************/
//Slot
void Widget::onNewPortList(QStringList portName)
{
    /*Insert default comboBox text*/
    portName.insert(0,COMBOBOX_DEFAULT_TEXT);

    qDebug() << __LINE__ << ":" << "New portNameList:" << portName;

    ui->comboBoxList->blockSignals(true);

    /**
     * comboBoxList and comboBoxListLast must be updated together,
     * overwise the comboBox in the onChooseSerialPortLast and onChooseSerialPort
     * are not the latest value.
    */

    ui->comboBoxList->clear();
    ui->comboBoxList->addItems(portName);
    ui->comboBoxListLast->clear();
    ui->comboBoxListLast->addItems(portName);
    ui->comboBoxList->blockSignals(false);

    /*Check if the comboBoxList needs to be updated, such as locking the serial port display,but the serial port is unplugged */
    if(portName.indexOf(currentSerialPortText) != -1)//当前的COM 还存在不
    {
        ui->comboBoxList->setCurrentText(currentSerialPortText);
        qDebug() << __LINE__ << ":" << "currentSerialPortText:" << currentSerialPortText;
    }else
    {
        ui->comboBoxList->setCurrentText(COMBOBOX_DEFAULT_TEXT);
        qDebug() << __LINE__ << ":" << "currentSerialPortText:"" ";
    }


     /*Check if the comboBoxList needs to be updated, such as locking the serial port display,but the serial port is unplugged */
    if(portName.indexOf(currentSerialPortLastText) != -1)
    {
        /*with changed index singal*/
        ui->comboBoxListLast->setCurrentText(currentSerialPortLastText);
        qDebug() << __LINE__ << ":" << "currentSerialPortLastText:" << currentSerialPortLastText;
    }else
    {
        /*with changed index singal*/
        ui->comboBoxListLast->setCurrentText(COMBOBOX_DEFAULT_TEXT);
        qDebug() << __LINE__ << ":" << "currentSerialPortLastText:"" " ;

    }

    /*2. Update the Label by Slots*/

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

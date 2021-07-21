#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QDesktopWidget>
#include <QRect>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    /*Forms without maxinize and minimize buttons, and title bar*/
    this->setWindowFlags(windowFlags()|Qt::FramelessWindowHint|Qt::WindowTitleHint);
    /*Translucent, transparency is 0.8*/
    this->setWindowOpacity(0.8);

    /*Install QEvent Filters*/
    ui->label->installEventFilter(this);
    ui->lineEdit->installEventFilter(this);


    ui->comboBoxList->setStyleSheet("QComboBox{color:black; background-color:red}"
                                    "QComboBox::drop-arrow{background:rgb(0,245,255);}"
                                    "QComboBox::pressed{background:#696969;}");/*more refer to : https://www.sojson.com/rgb.html and https://blog.csdn.net/xiaopei_yan/article/details/107404698*/

    ui->label->setStyleSheet("background:green");

    openSerialPort();

    connect(this, SIGNAL(onLabeltoLineText(QString)), this, SLOT(onUpdateLabel(QString)));
}

Widget::~Widget()
{
    qDebug() << __LINE__ << ":~Widget\r\n";
    delete wPortList;
    delete ui;
}

/*************************************** Mouse Event [Begin] ***************************************************/
//SLOT
void Widget::onUpdateLabel(QString strText)
{
    qDebug() << strText;
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
    }else if (obj == ui->lineEdit) {
        if(evt->type() == QEvent::KeyPress)
        {
            QKeyEvent *keyevt = static_cast<QKeyEvent*>(evt);
            if( (keyevt->key() == Qt::Key_Return) /*Enter key for large keyboard*/||
                (keyevt->key() == Qt::Key_Escape) /*ESC*/||
                (keyevt->key() == Qt::Key_Enter) /*Enter key for small keyboard*/)
            {
                /*Cumstomized Signals*/
                emit onLabeltoLineText("stackedWidget1");
                return true;
            }
        }else if (evt->type() == QEvent::FocusOut) {
            /*Cumstomized Signals*/
            emit onLabeltoLineText("stackedWidget1");
            return true;
        }
    }
    return QWidget::eventFilter(obj, evt);
}
/*************************************** Mouse Event [End] *****************************************************/


/*************************************** Serial Port Operation [Begin] *****************************************/

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

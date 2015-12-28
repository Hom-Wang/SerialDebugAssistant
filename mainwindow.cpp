#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "serialScope.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QKeyEvent>

//#pragma execution_character_set("UTF-8")

int recvCount = 0;
int sendCount = 0;

QSerialPort *serial;
QStringList portList;
QList<QAction *> pActionPort;
QActionGroup *pActionGroupPort;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
}

void MainWindow::initWindow()
{
    ui->statusBar->showMessage("  serial port not open ...");

    // Find available port
    QList<int>  portNumber;
    QStringList portDisplayName;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portList += info.portName();
        portDisplayName += info.portName() + " - " + info.description();
        portNumber += info.portName().remove("COM", Qt::CaseInsensitive).toInt();
    }
    for(int i = 0; i < portNumber.size() - 1; i++) {
        for(int j = 0; j < portNumber.size() - i - 1; j++) {
            if(portNumber[j] > portNumber[j + 1]) {
                portNumber.swap(j, j + 1);
                portList.swap(j, j + 1);
                portDisplayName.swap(j, j + 1);
            }
        }
    }

    pActionGroupPort = new QActionGroup(this);
    for(int i = 0; i < portList.size(); i++) {
        pActionPort += new QAction(portDisplayName[i], pActionGroupPort);
        pActionPort[i]->setCheckable(true);
    }
    ui->menuSerial_Port->addActions(pActionPort);

    // set menuBoud_Rate
    QList<QAction *> pActionBaud;
    QActionGroup *pActionGroupBaud = new QActionGroup(this);
    QStringList baudRateList;
    baudRateList << "9600" << "19200" << "38400" << "57600" << "115200" << "460800" << "921600";
    for(int i = 0; i < baudRateList.size(); i++) {
        pActionBaud += new QAction(baudRateList[i], pActionGroupBaud);
        pActionBaud[i]->setCheckable(true);
    }
    pActionBaud[4]->setChecked(true);
    ui->menuBoud_Rate->addActions(pActionBaud);

    // default setting
    serial = new QSerialPort(this);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    // connect
    connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()));
    connect(pActionGroupPort, SIGNAL(triggered(QAction*)), SLOT(onPortNameChanged(QAction*)));
    connect(pActionGroupBaud, SIGNAL(triggered(QAction*)), SLOT(onBaudRateChanged(QAction*)));
    connect(ui->actSerial_Scope, SIGNAL(triggered()), SLOT(callSerialScope()));
    connect(ui->actCube_Viewer,  SIGNAL(triggered()), SLOT(callCubeViewer()));
    connect(ui->actFile_Sender,  SIGNAL(triggered()), SLOT(callFileSender()));
    connect(ui->actReflash_List, SIGNAL(triggered()), this, SLOT(reflashPortList()));
    connect(ui->pushButton_Send, SIGNAL(clicked()), this, SLOT(onPushButton_Send_clicked()));
}

void MainWindow::reflashPortList()
{
    closeSerialPort();

    for(int i = 0; i < pActionPort.size(); i++) {
        ui->menuSerial_Port->removeAction(pActionPort[i]);
    }

    pActionPort.clear();

    QList<int>  tPortNumber;
    QStringList tPortList;
    QStringList tPortDisplayName;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        tPortList += info.portName();
        tPortDisplayName += info.portName() + " - " + info.description();
        tPortNumber += info.portName().remove("COM").toInt();
    }
    for(int i = 0; i < tPortNumber.size() - 1; i++) {
        for(int j = 0; j < tPortNumber.size() - i - 1; j++) {
            if(tPortNumber[j] > tPortNumber[j + 1]) {
                tPortNumber.swap(j, j + 1);
                tPortList.swap(j, j + 1);
                tPortDisplayName.swap(j, j + 1);
            }
        }
    }

    for(int i = 0; i < tPortList.size(); i++) {
        pActionPort += new QAction(tPortDisplayName[i], pActionGroupPort);
        pActionPort[i]->setCheckable(true);
    }
    ui->menuSerial_Port->addActions(pActionPort);

    portList = tPortList;
}

void MainWindow::onPortNameChanged( QAction *action )
{
    closeSerialPort();
    serial->setPortName(action->text().split(" ").at(0));
    openSerialPort();
    showStatusBar_Message();
}

void MainWindow::onBaudRateChanged( QAction *action )
{
    serial->setBaudRate((qint32)action->text().toInt());
    showStatusBar_Message();
}

void MainWindow::openSerialPort( void )
{
    if(serial->open(QIODevice::ReadWrite)) {
        serial->write("\n");
        ui->textBrowser_Recv->clear();
    }
}

void MainWindow::closeSerialPort( void )
{
    if(serial->isOpen())
        serial->close();
}

void MainWindow::serialRecv()
{
    QString debug_info;
    static QByteArray recvBuf;

    if(serial->isOpen()) {
        QByteArray recvData = serial->readAll();
        recvBuf.append(recvData);

        do {
            int bufLens    = recvBuf.size();
            int indexFirst = recvBuf.indexOf('S');
            if((indexFirst >= 0) && (bufLens > indexFirst + 6)) {
                uint8_t type = (uint8_t)((recvBuf.data()[indexFirst + 1]) & 0xE0);      // get data type
                uint8_t nByte = (uint8_t)((recvBuf.data()[indexFirst + 1]) & 0x1F);     // get data lens
                uint8_t packetLens = nByte + 6;     // include header, lens, checksum, '\r', '\n'
                if(bufLens > indexFirst + packetLens - 1) {
                    QByteArray packet = recvBuf.mid(indexFirst, packetLens);
                    uint8_t *data = (uint8_t*)packet.data();
                    if((data[0] == 'S') && (data[packetLens - 2] == '\r') && (data[packetLens - 1] == '\n')) {
                        if(true) {  // check checksum
                            SerialScope.updateSignal(data + 2, type, nByte);
                            debug_info += "signal = ";
                            debug_info += QString::number(SerialScope.signal[0]);
                            debug_info += "\t";
                            debug_info += QString::number(SerialScope.signal[1]);
                            debug_info += "\t";
                            debug_info += QString::number(recvBuf.size());
                            debug_info += "\n";
                        }
                    }
                    recvBuf.remove(0, indexFirst + packetLens - 1);
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        } while(true);

        ui->textBrowser_Recv->insertPlainText(debug_info);
        ui->textBrowser_Recv->moveCursor(QTextCursor::End);
    }
}

void MainWindow::addTextBrowserInfo( const QString info )
{
    ui->textBrowser_Recv->insertPlainText(info);
    ui->textBrowser_Recv->moveCursor(QTextCursor::End);
}

void MainWindow::onPushButton_Send_clicked()
{
    if(serial->isOpen()) {
        QString str = ui->lineEdit_Send->text();
        QByteArray sendData = str.toLatin1();
        serial->write(sendData);
        sendCount += sendData.size();
        ui->lineEdit_Send->clear();
    }
}

void MainWindow::showStatusBar_Message()
{
    ui->statusBar->showMessage("  " + serial->portName() + ", " + QString::number(serial->baudRate()), 0);
}

void MainWindow::keyPressEvent( QKeyEvent* e )
{
    switch(e->key()) {
        case Qt::Key_Backspace:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Up:
        case Qt::Key_Down:
            break;
        default:
            QString keyStr = e->text();
            QByteArray sendData = keyStr.toLatin1();
            serial->write(sendData);
    }
}

void MainWindow::callSerialScope()
{
    if(ui->actSerial_Scope->isChecked() == true) {
        SerialScope.show();
    }
    else {
        SerialScope.close();
//        delete SerialScope;
    }
}

void MainWindow::callCubeViewer()
{
    if(ui->actCube_Viewer->isChecked() == true) {
        CubeViewer.show();
    }
    else {
        CubeViewer.close();
//        delete CubeViewer;
    }
}

void MainWindow::callFileSender()
{
    if(ui->actFile_Sender->isChecked() == true) {

    }
    else {

    }
}

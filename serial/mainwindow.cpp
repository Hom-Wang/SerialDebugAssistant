#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDebug>

QSerialPort *serial;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QStringList portInfo;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        portInfo += info.portName()/* + " : " + info.description()*/;
    }
    qDebug() << portInfo << endl
             << "available ports = " << portInfo.size();

    serial = new QSerialPort(this);
    serial->setPortName(portInfo[0]);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    if(serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Serial is open" << endl;
        connect(serial, SIGNAL(readyRead()), this, SLOT(serialRecv()));

        QByteArray sendData;
        for(sendData[0] = 'A'; sendData[0] <= 'z'; sendData[0] = sendData[0] + 1) {
//            qDebug() << "send->" + sendData;
            serial->write(sendData);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
    serial->close();
}

void MainWindow::serialRecv()
{
    QByteArray serialData;

    serialData = serial->readAll();
    ui->label->setText(serialData);
    qDebug() << serialData;
}

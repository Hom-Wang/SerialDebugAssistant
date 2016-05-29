#include "serialscope.h"
#include "ui_serialscope.h"

#include "kfile.h"

#include <QKeyEvent>

/*
 * Add Function
 * 1. display mode, by Data or by Time
 * 2. change scal & zoom
 * 3. save to .mat / excel
 * 4. display channel(color), data, type & runtimes
 * 5. display sample rate
*/

SerialScope::SerialScope( QWidget *parent ) : QDialog(parent), ui(new Ui::SerialScope)
{
    ui->setupUi(this);

    // init parameter
    this->plotState = false;
    this->runTimes  = 0;

    // init customPlot
    const QColor colorTable[MAX_CHANNEL] = {
        Qt::red,      Qt::green,    Qt::blue,        Qt::cyan,
        Qt::magenta,  Qt::yellow,   Qt::darkRed,     Qt::darkGreen,
        Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow,
        Qt::red,      Qt::green,    Qt::blue,        Qt::cyan,
        Qt::magenta,  Qt::yellow,   Qt::darkRed,     Qt::darkGreen,
        Qt::darkBlue, Qt::darkCyan, Qt::darkMagenta, Qt::darkYellow
    };

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectPlottables);
    ui->customPlot->xAxis->setLabel("time");
    ui->customPlot->xAxis->setRange(0, 1000);
    ui->customPlot->yAxis->setLabel("data");
    ui->customPlot->yAxis->setRange(-10000, 10000);
    ui->customPlot->axisRect()->setupFullAxesBox();

    for(uint8_t i = 0; i < (MAX_CHANNEL << 1); i++) {
        ui->customPlot->addGraph();
        if(i < MAX_CHANNEL) {
            ui->customPlot->graph(i)->setPen(QPen(colorTable[i]));
        }
        else {
            ui->customPlot->graph(i)->setPen(QPen(colorTable[i - MAX_CHANNEL]));
            ui->customPlot->graph(i)->setLineStyle(QCPGraph::lsNone);
            ui->customPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssDisc);
        }
    }

    // connect
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(updateCustomPlot()));
    dataTimer.start(0);
}

SerialScope::~SerialScope()
{
    delete ui;
}

void SerialScope::setSignal( QCustomPlot *customPlot, int index, double data, double xAxis )
{
    customPlot->graph(index)->addData(xAxis, data);                 // add data to lines
    customPlot->graph(index + MAX_CHANNEL)->clearData();
    customPlot->graph(index + MAX_CHANNEL)->addData(xAxis, data);   // set data of dots

    this->signalVect[index].append(data);
}

void SerialScope::updateSignal( void *pData, uint8_t type, uint8_t channelLens )
{
    this->runTimes++;
    switch(type) {
        case KSerial_Mode_INT8:
            for(uint8_t i = 0; i < this->channel; i++)
                setSignal(ui->customPlot, i, ((int8_t*)pData)[i], this->runTimes);
            break;

        case KSerial_Mode_INT16:
            this->channel = channelLens >> 1;
            for(uint8_t i = 0; i < this->channel; i++)
                setSignal(ui->customPlot, i, ((int16_t*)pData)[i], this->runTimes);
            break;

        case KSerial_Mode_INT32:
            this->channel = channelLens >> 2;
            for(uint8_t i = 0; i < this->channel; i++)
                setSignal(ui->customPlot, i, ((int32_t*)pData)[i], this->runTimes);
            break;

        case KSerial_Mode_INT64:
            this->channel = channelLens >> 3;
            for(uint8_t i = 0; i < this->channel; i++)
                setSignal(ui->customPlot, i, ((int64_t*)pData)[i], this->runTimes);
            break;

        case KSerial_Mode_FLOAT32:
            this->channel = channelLens >> 2;
            for(uint8_t i = 0; i < this->channel; i++)
                setSignal(ui->customPlot, i, ((float*)pData)[i], this->runTimes);
            break;

        case KSerial_Mode_FLOAT64:
            this->channel = channelLens >> 3;
            for(uint8_t i = 0; i < this->channel; i++)
                setSignal(ui->customPlot, i, ((double*)pData)[i], this->runTimes);
            break;
    }
    this->plotState = true;
}

void SerialScope::updateCustomPlot()
{
    if(this->plotState == true) {
        ui->customPlot->xAxis->setRange(this->runTimes + 50, 1000, Qt::AlignRight);
        ui->customPlot->replot();
        this->plotState = false;
    }
}

void SerialScope::saveSignal( void )
{
    uint16_t *pSignalVect = NULL;
    uint32_t sampleRate   = 100;
    uint64_t dataLens     = this->signalVect[0].count();

    KFile kFile("KitSprout.txt");
    kFile.setFileHeader(sampleRate, dataLens << 1);

    for(uint8_t i = 0; i < this->channel; i++) {
        pSignalVect = this->signalVect[i].data();
        kFile.addSignal(i + 1, pSignalVect);
    }
}

void SerialScope::keyPressEvent( QKeyEvent* e )
{
    switch(e->key()) {
        case Qt::Key_S:
            saveSignal();
            break;
        default:
            break;
    }
}

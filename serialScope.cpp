#include "serialScope.h"
#include "ui_serialScope.h"

/*
 * Add Function
 * 1. display mode, by Data or by Time
 * 2. change scal & zoom
 * 3. save to .mat / excel
 * 4. display channel(color), data, type & runtimes
*/

SerialScope::SerialScope( QWidget *parent ) :
    QDialog(parent),
    ui(new Ui::SerialScope)
{
    ui->setupUi(this);

    this->plotState = false;
    this->runTimes = 0;
    this->signal[0] = 0;
    this->signal[1] = 0;
    this->signal[2] = 0;

    ui->customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectPlottables);
    ui->customPlot->xAxis->setLabel("time");
    ui->customPlot->xAxis->setRange(0, 1000);
    ui->customPlot->yAxis->setLabel("data");
    ui->customPlot->yAxis->setRange(-30000, 30000);
    ui->customPlot->axisRect()->setupFullAxesBox();

    addSignal(ui->customPlot, 0,  Qt::red);         // red
    addSignal(ui->customPlot, 2,  Qt::green);       // green
    addSignal(ui->customPlot, 4,  Qt::blue);        // blue
    addSignal(ui->customPlot, 6,  Qt::cyan);        // cyan
    addSignal(ui->customPlot, 8,  Qt::magenta);     // magenta
    addSignal(ui->customPlot, 10, Qt::yellow);      // yellow
    addSignal(ui->customPlot, 12, Qt::darkRed);     // darkRed
    addSignal(ui->customPlot, 14, Qt::darkGreen);   // darkGreen
    addSignal(ui->customPlot, 16, Qt::darkBlue);    // darkBlue
    addSignal(ui->customPlot, 18, Qt::darkCyan);    // darkCyan
    addSignal(ui->customPlot, 20, Qt::darkMagenta); // darkMagenta
    addSignal(ui->customPlot, 22, Qt::darkYellow);  // darkYellow

    addSignal(ui->customPlot, 24, Qt::red);         // red
    addSignal(ui->customPlot, 26, Qt::green);       // green
    addSignal(ui->customPlot, 28, Qt::blue);        // blue
    addSignal(ui->customPlot, 30, Qt::cyan);        // cyan
    addSignal(ui->customPlot, 32, Qt::magenta);     // magenta
    addSignal(ui->customPlot, 34, Qt::yellow);      // yellow
    addSignal(ui->customPlot, 36, Qt::darkRed);     // darkRed
    addSignal(ui->customPlot, 38, Qt::darkGreen);   // darkGreen
    addSignal(ui->customPlot, 40, Qt::darkBlue);    // darkBlue
    addSignal(ui->customPlot, 42, Qt::darkCyan);    // darkCyan
    addSignal(ui->customPlot, 44, Qt::darkMagenta); // darkMagenta
    addSignal(ui->customPlot, 46, Qt::darkYellow);  // darkYellow

    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(updateCustomPlot()));
    dataTimer.start(0);
}

SerialScope::~SerialScope()
{
    delete ui;
}

void SerialScope::addSignal( QCustomPlot *customPlot, int index, QColor color )
{
    customPlot->addGraph(); // line
    customPlot->graph(index)->setPen(QPen(color));
    customPlot->addGraph(); // dot
    customPlot->graph(index + 1)->setPen(QPen(color));
    customPlot->graph(index + 1)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(index + 1)->setScatterStyle(QCPScatterStyle::ssDisc);
}

void SerialScope::setSignal( QCustomPlot *customPlot, int index, double data, double xAxis )
{
    customPlot->graph(index)->addData(xAxis, data);         // add data to lines
    customPlot->graph(index + 1)->clearData();
    customPlot->graph(index + 1)->addData(xAxis, data);     // set data of dots
    // remove data of lines that's outside visible range:
    //customPlot->graph(index)->removeDataBefore(xAxis - window);
}

void SerialScope::updateSignal( void *recvData, uint8_t type, uint8_t lens )
{
    this->runTimes++;
    switch(type) {
    case 0x00:  // INT8
        for(uint8_t i = 0; i < lens; i++) {
            this->signal[i] = ((int8_t*)recvData)[i];
            setSignal(ui->customPlot, i << 1, this->signal[i], this->runTimes);
        }
        break;
    case 0x20:  // INT16
        lens = lens >> 1;
        for(uint8_t i = 0; i < lens; i++) {
            this->signal[i] = ((int16_t*)recvData)[i];
            setSignal(ui->customPlot, i << 1, this->signal[i], this->runTimes);
        }
        break;
    case 0x40:  // INT32
        lens = lens >> 2;
        for(uint8_t i = 0; i < lens; i++) {
            this->signal[i] = ((int32_t*)recvData)[i];
            setSignal(ui->customPlot, i << 1, this->signal[i], this->runTimes);
        }
        break;
    case 0x80:  // INT64
        lens = lens >> 3;
        for(uint8_t i = 0; i < lens; i++) {
            this->signal[i] = ((int64_t*)recvData)[i];
            setSignal(ui->customPlot, i << 1, this->signal[i], this->runTimes);
        }
        break;
    case 0xC0:  // FLOAT32
        lens = lens >> 2;
        for(uint8_t i = 0; i < lens; i++) {
            this->signal[i] = ((float*)recvData)[i];
            setSignal(ui->customPlot, i << 1, this->signal[i], this->runTimes);
        }
        break;
    case 0xE0:  // FLOAT64
        lens = lens >> 3;
        for(uint8_t i = 0; i < lens; i++) {
            this->signal[i] = ((double*)recvData)[i];
            setSignal(ui->customPlot, i << 1, this->signal[i], this->runTimes);
        }
        break;
    }

    plotState = true;
}

void SerialScope::updateCustomPlot()
{
    if(plotState == true) {
        ui->customPlot->xAxis->setRange(this->runTimes + 50, 1000, Qt::AlignRight);
        ui->customPlot->replot();
        plotState = false;
    }
}

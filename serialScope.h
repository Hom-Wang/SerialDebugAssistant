#ifndef SERIALSCOP_H
#define SERIALSCOP_H

#include "qcustomplot.h"

#include <QDialog>

namespace Ui {
class SerialScope;
}

class SerialScope : public QDialog
{
    Q_OBJECT

public:
    explicit SerialScope(QWidget *parent = 0);
    ~SerialScope();

    double signal[24];
    void updateSignal( void *recvData, uint8_t type, uint8_t lens );

private:
    bool plotState;
    double runTimes;

private slots:
    void addSignal( QCustomPlot *customPlot, int index, QColor color );
    void setSignal( QCustomPlot *customPlot, int index, double data, double xAxis );
    void updateCustomPlot();

private:
    Ui::SerialScope *ui;
    QTimer dataTimer;
};

#endif // SERIALSCOP_H

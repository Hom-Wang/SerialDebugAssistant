#ifndef SERIALSCOP_H
#define SERIALSCOP_H

#include "qcustomplot.h"

#include <QDialog>
#include <QThread>
#include <QVector>
#include <QKeyEvent>

#define MAX_CHANNEL 24  // 24 bytes

typedef enum {
    KSerial_Mode_INT8    = 0x00,    // 8'b000x_xxxx
    KSerial_Mode_INT16   = 0x20,    // 8'b001x_xxxx
    KSerial_Mode_INT32   = 0x40,    // 8'b010x_xxxx
    KSerial_Mode_INT64   = 0x80,    // 8'b011x_xxxx
    KSerial_Mode_FLOAT32 = 0xC0,    // 8'b110x_xxxx
    KSerial_Mode_FLOAT64 = 0xE0,    // 8'b111x_xxxx
} KSerial_Mode_TypeDef;

namespace Ui {
class SerialScope;
}

class SerialScope : public QDialog
{
    Q_OBJECT

public:
    explicit SerialScope( QWidget *parent = 0 );
    ~SerialScope();

    uint8_t channel;
    void updateSignal( void *pData, uint8_t type, uint8_t channelLens );

public slots:
    void updateCustomPlot();

private:
    bool plotState;
    double runTimes;
    QVector<uint16_t> signalVect[MAX_CHANNEL];

private slots:
    void setSignal( QCustomPlot *customPlot, int index, double data, double xAxis );
    void keyPressEvent( QKeyEvent* e );
    void saveSignal( void );

private:
    Ui::SerialScope *ui;
    QTimer dataTimer;
};

#endif // SERIALSCOP_H

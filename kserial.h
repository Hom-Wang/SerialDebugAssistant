#ifndef KSERIAL_H
#define KSERIAL_H

#include <QSerialPort>

class KSerial : public QSerialPort {

public:
    KSerial();
    ~KSerial();

private:
    void recv();

};

#endif // KSERIAL_H

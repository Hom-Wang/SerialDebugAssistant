#ifndef KFILE_H
#define KFILE_H

#include <QFile>
#include <QString>
#include <stdint.h>

class KFile {

public:
    KFile( const QString _fileName );
    ~KFile();

    void setFileHeader( uint32_t sampleRate, uint64_t dataLens );
    void addSignal( uint8_t channel, uint16_t *pData );
    void openFile( void );

private:
    QFile file;
    uint8_t headerInfo[16];
    uint64_t dataLens;

};

#endif // KFILE_H

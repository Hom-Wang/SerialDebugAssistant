#include "kfile.h"

#include <QFile>

KFile::KFile( const QString _fileName )
{
    this->headerInfo[0]  = 'K';
    this->headerInfo[1]  = '-';
    this->headerInfo[2]  = 'L';
    this->headerInfo[3]  = 0;
    this->headerInfo[4]  = 0;
    this->headerInfo[5]  = 0;
    this->headerInfo[6]  = 0;
    this->headerInfo[7]  = 0;
    this->headerInfo[8]  = 0;
    this->headerInfo[9]  = 'S';
    this->headerInfo[10] = 0;
    this->headerInfo[11] = 0;
    this->headerInfo[12] = 0;
    this->headerInfo[13] = 0;
    this->headerInfo[14] = 'E';
    this->headerInfo[15] = 'D';
    this->file.setFileName(_fileName);
}

KFile::~KFile()
{
    this->file.close();
}

void KFile::setFileHeader( uint32_t sampleRate, uint64_t dataLens )
{
    this->dataLens = dataLens;

    this->headerInfo[3]  = dataLens >> 40;
    this->headerInfo[4]  = dataLens >> 32;
    this->headerInfo[5]  = dataLens >> 24;
    this->headerInfo[6]  = dataLens >> 16;
    this->headerInfo[7]  = dataLens >> 8;
    this->headerInfo[8]  = dataLens;

    this->headerInfo[10] = sampleRate >> 24;
    this->headerInfo[11] = sampleRate >> 16;
    this->headerInfo[12] = sampleRate >> 8;
    this->headerInfo[13] = sampleRate;

    this->file.open(QIODevice::ReadWrite);
    this->file.write((char*)(this->headerInfo), 16);
    this->file.close();
}

void KFile::addSignal( uint8_t channel, uint16_t *pData )
{
    uint8_t channelStr[8] = {0};

    this->file.open(QIODevice::Append);
    channelStr[0] = '\r';
    channelStr[1] = '\n';
    channelStr[2] = 'C';
    channelStr[3] = 'H';
    channelStr[4] = (uint16_t)(channel / 10) + 48;
    channelStr[5] = (uint16_t)(channel % 10) + 48;
    channelStr[6] = '\r';
    channelStr[7] = '\n';
    this->file.write((char*)channelStr, 8);
    this->file.atEnd();
    this->file.write((char*)pData, this->dataLens);
    this->file.close();
}

void KFile::openFile( void )
{

}

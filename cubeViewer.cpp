#include "cubeViewer.h"
#include "ui_cubeViewer.h"

CubeViewer::CubeViewer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CubeViewer)
{
    ui->setupUi(this);
}

CubeViewer::~CubeViewer()
{
    delete ui;
}

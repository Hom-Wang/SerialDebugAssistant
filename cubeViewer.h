#ifndef CUBEVIEWER_H
#define CUBEVIEWER_H

#include <QDialog>

namespace Ui {
class CubeViewer;
}

class CubeViewer : public QDialog
{
    Q_OBJECT

public:
    explicit CubeViewer(QWidget *parent = 0);
    ~CubeViewer();

private slots:


private:
    Ui::CubeViewer *ui;

};

#endif // CUBEVIEWER_H

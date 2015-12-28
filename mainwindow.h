#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialScope.h"
#include "cubeViewer.h"

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    SerialScope SerialScope;
    CubeViewer CubeViewer;
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    void addTextBrowserInfo( const QString info );

private slots:
    void initWindow();
    void serialRecv();
    void reflashPortList();
    void onPortNameChanged( QAction *action );
    void onBaudRateChanged( QAction *action );
    void openSerialPort( void );
    void closeSerialPort( void );
    void showStatusBar_Message();
    void onPushButton_Send_clicked();
    void keyPressEvent( QKeyEvent* e );

    void callSerialScope();
    void callCubeViewer();
    void callFileSender();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H

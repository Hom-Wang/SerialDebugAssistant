#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "serialscope.h"
#include "viewer3d.h"

#include <QMainWindow>
#include <QKeyEvent>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow( QWidget *parent = 0 );
    ~MainWindow();

    SerialScope SerialScope;
    Viewer3D Viewer3D;
    void addTextBrowserInfo( const QString info );

private slots:
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
    void callViewer3D();
    void callFileSender();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H

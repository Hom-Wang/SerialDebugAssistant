#ifndef VIEWER3D_H
#define VIEWER3D_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

namespace Ui {
class Viewer3D;
}

class Viewer3D : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit Viewer3D( QWidget *parent = 0 );
    ~Viewer3D();

private slots:


private:
    Ui::Viewer3D *ui;

    QOpenGLBuffer vbo;

protected:
    void initializeGL( void );
    void paintGL( void );
//    void resizeGL( int width, int height );

    void makeObject( void );
};

#endif // VIEWER3D_H

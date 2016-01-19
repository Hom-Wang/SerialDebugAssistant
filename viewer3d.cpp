#include "viewer3d.h"
#include "ui_viewer3d.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

Viewer3D::Viewer3D( QWidget *parent ) : QOpenGLWidget(parent), ui(new Ui::Viewer3D)
{
    ui->setupUi(this);

}

Viewer3D::~Viewer3D()
{
    delete ui;
}

void Viewer3D::initializeGL()
{
    initializeOpenGLFunctions();

    makeObject();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

}

void Viewer3D::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
}
/*
void Viewer3D::resizeGL( int width, int height )
{

}
*/
void Viewer3D::makeObject( void )
{

}

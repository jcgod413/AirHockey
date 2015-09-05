#ifndef IMAGEGLVIEW_H
#define IMAGEGLVIEW_H

#include <QGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include "ahr.h"

class ImageGLView : public QGLWidget
{
    Q_OBJECT

public:
    explicit ImageGLView(QWidget *parent = 0);
    ~ImageGLView();
    void imageLoad(QImage);

private:
    int startX;
    int startY;
    int endX;
    int endY;

    int painterWidth;
    int painterHeight;

    QImage frameImage;

    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void signalMouseLeftClick(QPoint);
    void signalMouseMove(QPoint);
    void signalMouseRelease(QPoint);
};

#endif // IMAGEGLVIEW_H

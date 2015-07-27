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

private:
    int startX;
    int startY;
    int endX;
    int endY;

    int painterWidth;
    int painterHeight;

    int mousePosX;
    int mousePosY;
    int boardAreaClick;

    bool isMouseClicked;
    bool isBoardArea;
    bool isRectangleReady;

    int redMax;
    int redMin;
    int greenMax;
    int greenMin;
    int blueMax;
    int blueMin;

    int leftTopX;
    int leftTopY;
    int leftBottomX;
    int leftBottomY;
    int rightTopX;
    int rightTopY;
    int rightBottomX;
    int rightBottomY;

    double gradientA;   // Left Top Point와 Right Top Point의 기울기
    double gradientB;   // Right Top Point와 Right Bottom Point의 기울기
    double gradientC;   // Right Bottom Point와 Left Bottom Point의 기울기
    double gradientD;   // Right Bottom Point와 Left Bottom Point의 기울기

    QPixmap frameImage;

    int getBoundStartX(int y);
    int getBoundEndX(int y);

    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void slotImageLoad(QPixmap);
    void slotResetColor();
    void slotSetBoardArea();
};

#endif // IMAGEGLVIEW_H

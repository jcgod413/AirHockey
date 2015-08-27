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

    int mousePosX;
    int mousePosY;
    int boardAreaClick;

    bool isMouseClicked;
    bool isBoardArea;
    bool isRectangleReady;
    bool isRectangleBoardArea;
    bool isRectangleBoardClick;

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

    int erodeNum;
    int dilateNum;

    QImage frameImage;

    QPoint ballPos;
    bool ballFound;
    double ballGradient;

    bool isBallMoving;
    BallDirection ballDirection;

    int dir[8][2] = { {-1,-1}, {0, -1}, {1, -1}, {1, 0},
                      {1, 1},  {0, 1}, {-1, 1}, {-1, 0}};

    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

signals:
    void signalDraggedImage(int, int);
    void signalBoardAreaPoint(int, int, int);
    void signalBoardAreaReady(bool);

public slots:
    void slotSetBoardArea();
    void slotSetRectangleBoardArea();
    void slotRectangleReady(bool);
    void slotBoardArea(bool);
    void slotFindBall(QPoint);
    void slotPredictGradient(double);
    void slotBallMoving(bool, BallDirection);
};

#endif // IMAGEGLVIEW_H

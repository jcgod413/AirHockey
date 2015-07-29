#ifndef IMAGEGLVIEW_H
#define IMAGEGLVIEW_H

#include <QGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QColor>
#include "ahr.h"
#include "imageprocessing.h"

class ImageGLView : public QGLWidget
{
    Q_OBJECT

public:
    explicit ImageGLView(QWidget *parent = 0);
    ~ImageGLView();

private:
    ImageProcessing imageProcessing;

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

    int erodeNum;

    QPixmap frameImage;

    int dir[8][2] = { {-1,-1}, {0, -1}, {1, -1}, {1, 0},
                      {1, 1},  {0, 1}, {-1, 1}, {-1, 0}};

    void getBoundX(int y, int &startX, int &endX);

    virtual void paintEvent(QPaintEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);

public slots:
    void slotImageLoad(QPixmap);
    void slotResetColor();
    void slotSetBoardArea();
    void slotErodeNumChanged(int);
    void slotDilateNumChanged(int);
};

#endif // IMAGEGLVIEW_H

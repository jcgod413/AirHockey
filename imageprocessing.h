#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QTimer>
#include <QElapsedTimer>
#include <QPoint>
#include <QColor>
#include <QPainter>
#include "ahr.h"
#include "ball.h"
#include "robot.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = 0);
    ~ImageProcessing();
    void getThresholdImage(QImage*);
    void getBoundX(int y, int &startX, int &endX);
    void getObjectsPosition(QImage *frameImage);
    void initImageProcessing();
    QImage imageProcess(QImage*);
    void erode(QImage*);
    void dilate(QImage*);
    void predictCourse(QImage*);
    void drawFence();
    void ballTracking();
    QPoint predictPoint;
    void radioStateChanged(RadioState);

private:
    Ball *ball;
    Robot *robot;
    QPoint ballPos;
    QPoint robotPos;
    QPoint mousePos;
    QImage rawImage;
    QImage resultImage;
    RobotSide robotSide;
    RadioState radioState;

    bool isRectangleBoardMode;
    bool isRectangleGrabbing;
    bool isRectangleReady;
    bool isMouseClick;

    int toleranceBand;  // Mask Color 오차 범위

    int labelBall[SCREEN_HEIGHT][SCREEN_WIDTH];
    int labelRobot[SCREEN_HEIGHT][SCREEN_WIDTH];

    int dir[8][2] = { {0, -1}, {-1,-1}, {-1, 0}, {-1, 1}, 
                      {1, -1}, {1, 0}, {1, 1},  {0, 1} };
    struct Outline  {
        QPoint leftUpSide;
        QPoint rightDownSide;
    };

    bool isBoardAreaReady;

    unsigned char *imageData;
    unsigned char *rawImageData;

    int redMax;
    int redMin;
    int greenMax;
    int greenMin;
    int blueMax;
    int blueMin;

    QPoint leftTopPoint;
    QPoint rightBottomPoint;

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
    bool enableMorpology;

    double gradientA;   // Left Top Point와 Right Top Point의 기울기
    double gradientB;   // Right Top Point와 Right Bottom Point의 기울기
    double gradientC;   // Right Bottom Point와 Left Bottom Point의 기울기
    double gradientD;   // Right Bottom Point와 Left Bottom Point의 기울기

    QColor ballSignColor;
    QColor robotSignColor;

signals:
    void signalRectangleReady(bool);
    void signalBoardArea(bool);
    void signalFindBall(QPoint);
    void signalRenewBallColor(QColor, QColor);
    void signalRenewObjects(Ball*, Robot*);
    void signalImageProcessCompleted();

public slots:
    void slotDraggedImage(int, int);
    void slotResetMaskColor();
    void slotMorpologyEnable(bool);
    void slotBoardAreaReady(bool);

    void slotMouseClick(QPoint);
    void slotMouseMove(QPoint);
    void slotMouseRelease(QPoint);

    void slotSetRectangleBoardArea();
    void slotRobotSideChanged(int);

    void slotSendObjects();
};

#endif // IMAGEPROCESSING_H

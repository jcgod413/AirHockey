#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include <QElapsedTimer>
#include <QPoint>
#include <QColor>
#include "ahr.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = 0);
    ~ImageProcessing();
    void loadRawImage(QImage);
    void erode(QImage &sourceImage);
    void dilate(QImage &sourceImage);
    QImage getThresholdImage();
    void getBoundX(int y, int &startX, int &endX);
    QPoint getBallPosition(QImage *frameImage);
    inline void searchGroup(int groupNum, QPoint point,
                     QPoint *minPoint, QPoint *maxPoint);

private:
    QImage rawImage;

    int toleranceBand;  // Mask Color 오차 범위

    int label[SCREEN_HEIGHT][SCREEN_WIDTH];
    int dir[8][2] = { {0, -1}, {-1,-1}, {-1, 0}, {-1, 1}, 
                      {1, -1}, {1, 0}, {1, 1},  {0, 1}};
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

    QColor *ballColor;

signals:
    void signalRectangleReady(bool);
    void signalBoardArea(bool);
    void signalFindBall(QPoint);

public slots:
    void slotDraggedImage(int, int);
    void slotResetMaskColor();
    void slotBoardAreaPoint(int, int, int);
    void slotMorpologyEnable(bool);
    void slotBoardAreaReady(bool);
};

#endif // IMAGEPROCESSING_H

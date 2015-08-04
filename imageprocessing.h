#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include <QColor>
#include "ahr.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = 0);
    ~ImageProcessing();
    void loadRawImage(QImage);
    void erodeImage(QImage &sourceImage);
    QImage getThresholdImage();
    void getBoundX(int y, int &startX, int &endX);

private:
    QImage rawImage;

    int toleranceBand;  // Mask Color 오차

    int dir[8][2] = { {-1,-1}, {0, -1}, {1, -1}, {1, 0},
                      {1, 1},  {0, 1}, {-1, 1}, {-1, 0}};
    bool isBoardAreaReady;

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
signals:
    void signalRectangleReady(bool);
    void signalBoardArea(bool);

public slots:
    void slotDraggedImage(int, int);
    void slotResetMaskColor();
    void slotBoardAreaPoint(int, int, int);
};

#endif // IMAGEPROCESSING_H

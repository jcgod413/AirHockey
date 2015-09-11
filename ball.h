#ifndef BALL_H
#define BALL_H

#include <QWidget>
#include <QQueue>
#include <math.h>
#include "ahr.h"

class Ball : public QWidget
{
    Q_OBJECT

public:
    Ball();
    ~Ball();
    void updatePosition(QPoint);
    void updateInfo();

    bool isMoving;
    double gradient;
    double distance;
    BallDirection direction;
    QPoint predictPoint;
    QPoint previousPredictPoint;
    bool found();
    QPoint getPosition();

    QPoint startPoint;

    void updateColor(int, int, int);
    void resetColor();

    QColor getMaxColor();
    QColor getMinColor();

private:
    QQueue<QPoint> ballQueue;
    QPoint position;

    int redMax;
    int redMin;
    int greenMax;
    int greenMin;
    int blueMax;
    int blueMin;

    QColor maxColor;
    QColor minColor;
};

#endif // BALL_H

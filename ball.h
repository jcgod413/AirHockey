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
    void find(QPoint);
    void updateInfo();

    bool isMoving;
    double gradient;
    double distance;
    BallDirection direction;
    QColor maxColor;
    QColor minColor;
    QPoint predictPoint;
    QPoint previousPredictPoint;
    bool found();
    int getX();
    int getY();

    QPoint startPoint;

private:
    QQueue<QPoint> ballQueue;
    QPoint ballPosition;

};

#endif // BALL_H

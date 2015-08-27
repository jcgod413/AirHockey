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

private:
    QQueue<QPoint> ballQueue;
    QPoint ballPosition;
    double gradient;
    double distance;

signals:
    void signalPredictGradient(double);
    void signalBallMoving(bool, BallDirection);

public slots:
    void slotFindBall(QPoint);
    void predictCourse();
};

#endif // BALL_H

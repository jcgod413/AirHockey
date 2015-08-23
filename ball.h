#ifndef BALL_H
#define BALL_H

#include <QWidget>
#include <QQueue>

using namespace std;

class Ball
{
public:
    Ball();
    ~Ball();

private:
    QQueue<QPoint> ballQueue;
    QPoint ballPosition;
    double gradient;

signals:
    void signalPredictGradient(double);

public slots:
    void slotBallFound(QPoint);
    void slotPredictCourse();
};

#endif // BALL_H

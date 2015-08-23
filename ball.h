#ifndef BALL_H
#define BALL_H

#include <QWidget>
#include <QQueue>

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

signals:
    void signalPredictGradient(double);

public slots:
    void slotFindBall(QPoint);
    void slotPredictCourse();
};

#endif // BALL_H

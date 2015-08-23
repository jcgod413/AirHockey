#include "ball.h"

Ball::Ball() :
    ballPosition(0, 0)
{

}

Ball::~Ball()
{

}

void Ball::slotBallFound(QPoint _ballPosition)
{
    ballPosition = _ballPosition;

    if( ballQueue.size() >= 5 )
        ballQueue.dequeue();
    ballQueue.enqueue(ballPosition);
}

void Ball::slotPredictCourse()
{
    QPoint previousBallPosition = ballQueue.first();

    gradient = (double)(ballPosition.y() - previousBallPosition.y())
               / (double)(ballPosition.x() - previousBallPosition.x());

//    emit signalPredictGradient(gradient);
}

#include "ball.h"

/**
 * @brief Ball::Ball
 */
Ball::Ball() :
    ballPosition(0, 0)
{

}

/**
 * @brief Ball::~Ball
 */
Ball::~Ball()
{

}

/**
 * @brief Ball::slotFindBall
 * @param _ballPosition
 */
void Ball::slotFindBall(QPoint _ballPosition)
{
    ballPosition = _ballPosition;

    ballQueue.enqueue(ballPosition);

    if( ballQueue.size() > 5 ) {
        ballQueue.dequeue();
        predictCourse();
    }
}

/**
 * @brief Ball::predictCourse
 */
void Ball::predictCourse()
{
    QPoint previousBallPosition = ballQueue.first();

//    distance = sqrt(pow((double)(ballPosition.x() - previousBallPosition.x()), 2.0)
//                    + pow((double)(ballPosition.y() - previousBallPosition.y()), 2.0));
    distance = abs(ballPosition.x() - previousBallPosition.x())
                + abs(ballPosition.y() - previousBallPosition.y());

    if( distance > 5 )    {
        gradient = (double)(ballPosition.y() - previousBallPosition.y())
                   / (double)(ballPosition.x() - previousBallPosition.x());

        emit signalPredictGradient(gradient);
        emit signalBallMoving(true);
    }
    else    {
        emit signalBallMoving(false);
    }
}

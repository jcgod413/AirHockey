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

    // 정지해있는지 프레임간의 공의 거리 차이를 가지고 판단
//    distance = sqrt(pow((double)(ballPosition.x() - previousBallPosition.x()), 2.0)
//                    + pow((double)(ballPosition.y() - previousBallPosition.y()), 2.0));
    distance = abs(ballPosition.x() - previousBallPosition.x())
                + abs(ballPosition.y() - previousBallPosition.y());

    BallDirection direction;

    if( ballPosition.x() > previousBallPosition.x()
        && ballPosition.y() > previousBallPosition.y() )    {
        direction = SOUTH_EAST;
    }
    else if( ballPosition.x() > previousBallPosition.x()
             && ballPosition.y() < previousBallPosition.y() )    {
        direction = NORTH_EAST;
    }
    else if( ballPosition.x() < previousBallPosition.x()
             && ballPosition.y() < previousBallPosition.y() )    {
        direction = NORTH_WEST;
    }
    else    {
        direction = SOUTH_WEST;
    }

    if( distance > 10 )    {
        gradient = (double)(ballPosition.y() - previousBallPosition.y())
                   / (double)(ballPosition.x() - previousBallPosition.x());

        emit signalPredictGradient(gradient);
        emit signalBallMoving(true, direction);
    }
    else    {
        emit signalBallMoving(false, PAUSE);
    }
}

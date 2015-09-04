#include "ball.h"

/**
 * @brief Ball::Ball
 */
Ball::Ball() :
    isMoving(false),
    ballPosition(0, 0),
    gradient(0.0),
    distance(0)
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
void Ball::find(QPoint _ballPosition)
{
    ballPosition = _ballPosition;

    ballQueue.enqueue(ballPosition);
}

/**
 * @brief Ball::calculateInfo
 */
void Ball::updateInfo()
{
    QPoint previousBallPosition = ballQueue.first();

    if( ballQueue.size() > 5 ) {
        ballQueue.dequeue();
    }
    else    {
        return;
    }

    // 정지해있는지 프레임간의 공의 거리 차이를 가지고 판단
    distance = abs(ballPosition.x() - previousBallPosition.x())
                + abs(ballPosition.y() - previousBallPosition.y());

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

        isMoving = true;
    }
    else    {
        isMoving = false;
    }
}

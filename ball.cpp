#include "ball.h"

/**
 * @brief Ball::Ball
 */
Ball::Ball() :
    isMoving(false),
    position(0, 0),
    predictPoint(0, 0),
    previousPredictPoint(0, 0),
    gradient(0.0),
    distance(0),
    redMax(0),
    greenMax(0),
    blueMax(0),
    redMin(255),
    greenMin(255),
    blueMin(255)
{

}

/**
 * @brief Ball::~Ball
 */
Ball::~Ball()
{

}

QPoint Ball::getPosition()
{
    return position;
}

/**
 * @brief Ball::updateColor
 * @param red
 * @param green
 * @param blue
 */
void Ball::updateColor(int red, int green, int blue)
{
    redMax   = (redMax<red) ? red : redMax;
    redMin   = (redMin>red) ? red : redMin;
    greenMax = (greenMax<green) ? green : greenMax;
    greenMin = (greenMin>green) ? green : greenMin;
    blueMax  = (blueMax<blue) ? blue : blueMax;
    blueMin  = (blueMin>blue) ? blue : blueMin;

    maxColor = QColor(redMax, greenMax, blueMax);
    minColor = QColor(redMin, greenMin, blueMin);
}

/**
 * @brief Ball::resetColor
 */
void Ball::resetColor()
{
    redMax = greenMax = blueMax = 0;
    redMin = greenMin = blueMin = 255;

    maxColor = QColor(redMax, greenMax, blueMax);
    minColor = QColor(redMin, greenMin, blueMin);
}

/**
 * @brief Ball::updatePosition
 * @param position
 */
void Ball::updatePosition(QPoint position)
{
    this->position = position;

    ballQueue.enqueue(position);

    updateInfo();
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
    distance = abs(position.x() - previousBallPosition.x())
                + abs(position.y() - previousBallPosition.y());

    if( position.x() > previousBallPosition.x()
        && position.y() > previousBallPosition.y() )    {
        direction = SOUTH_EAST;
    }
    else if( position.x() > previousBallPosition.x()
             && position.y() < previousBallPosition.y() )    {
        direction = NORTH_EAST;
    }
    else if( position.x() < previousBallPosition.x()
             && position.y() < previousBallPosition.y() )    {
        direction = NORTH_WEST;
    }
    else    {
        direction = SOUTH_WEST;
    }

    if( distance > 16 )    {
        gradient = (double)(position.y() - previousBallPosition.y())
                   / (double)(position.x() - previousBallPosition.x());

        isMoving = true;
    }
    else    {
        isMoving = false;
    }
}

/**
 * @brief Ball::found
 * @return
 */
bool Ball::found()
{
    return (position.x()>0 && position.y()>0)
            ? true
            : false;
}

/**
 * @brief Ball::getMaxColor
 * @return
 */
QColor Ball::getMaxColor()
{
    return maxColor;
}

/**
 * @brief Ball::getMinColor
 * @return
 */
QColor Ball::getMinColor()
{
    return minColor;
}

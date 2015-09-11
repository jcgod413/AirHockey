#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent) :
    QObject(parent),
    ball(new Ball),
    robot(new Robot),
    radioState(RADIO_BALL),
    ballSignColor(0, 0, 255),
    robotSignColor(255, 0, 0),
    ballPos(0, 0),
    robotPos(0, 0),
    mousePos(0, 0),
    predictPoint(0, 0),
    isMouseClick(false),
    isBoardAreaReady(false),
    enableMorpology(false),
    isRectangleReady(false),
    isRectangleBoardMode(false),
    isRectangleGrabbing(false),
    leftTopX(0),
    leftTopY(0),
    leftBottomX(0),
    leftBottomY(0),
    rightTopX(0),
    rightTopY(0),
    rightBottomX(0),
    rightBottomY(0),
    toleranceBand(5),
    erodeNum(2),
    dilateNum(1)
{
    initImageProcessing();
}

/**
 * @brief ImageProcessing::~ImageProcessing
 */
ImageProcessing::~ImageProcessing()
{

}

/**
 * @brief ImageProcessing::initImageProcessing
 */
void ImageProcessing::initImageProcessing()
{
    qDebug("emit");
    // 객체 생성시간의 차이때문에 delay를 주기위하여 singleShot을 사용
    QTimer::singleShot(3000, this, SLOT(slotSendObjects()));
}

/**
 * @brief ImageProcessing::slotSendObjects
 */
void ImageProcessing::slotSendObjects()
{
    emit signalRenewObjects(ball, robot);
}

/**
 * @brief ImageProcessing::imageProcess
 * @param rawImage
 * @return
 */
QImage ImageProcessing::imageProcess(QImage *rawImage)
{
    this->rawImage = *rawImage;
    resultImage = *rawImage;

    // threshold
    getThresholdImage(&resultImage);

    // morpology
    if( enableMorpology == true )   {
        for(int i=0; i<erodeNum; i++)
            erode(&resultImage);
        for(int i=0; i<dilateNum; i++)
            dilate(&resultImage);
    }

    // get objects position
    getObjectsPosition(&resultImage);
    ball->updatePosition(ballPos);
    robot->updatePosition(robotPos);
    emit signalFindBall(ballPos);

    // predict
    predictCourse(&resultImage);

    // tracking
    ballTracking();
    if( ball->found() == true ) {
        emit signalImageProcessCompleted();
    }

    // draw fence
    drawFence();

    return resultImage;
}

/**
 * @brief ImageProcessing::drawFence
 */
void ImageProcessing::drawFence()
{
    if( (rightBottomPoint.x() > 0 && rightBottomPoint.y() > 0)
        && (rightBottomPoint.x() > leftTopPoint.x())
        && (rightBottomPoint.y() > leftTopPoint.y()) )
    {
        QPainter painter(&resultImage);
        painter.drawRect(QRect(leftTopPoint, rightBottomPoint));
    }
}

/**
 * @brief ImageProcessing::ballTracking
 */
void ImageProcessing::ballTracking()
{
    // To Do
/*
    if( robotDirection == LEFT_SIDE )   {
        // 예상 도달 지점 잡기
    }
    else if( robotDirection == RIGHT_SIDE ) {

    }
    */

}

/**
 * @brief ImageProcessing::getThresholdImage
 * @param dstImage
 */
void ImageProcessing::getThresholdImage(QImage *dstImage)
{
    int boundStartY = (leftTopY<rightTopY)
                     ? leftTopY
                     : rightTopY;
    int boundEndY = (leftBottomY>rightBottomY)
                    ? leftBottomY
                    : rightBottomY;

    *dstImage = rawImage;
    imageData = dstImage->bits();

    if( isBoardAreaReady == true )  {
        int startX = leftTopPoint.x();
        int startY = leftTopPoint.y();
        int endX = rightBottomPoint.x();
        int endY = rightBottomPoint.y();
        int loc;

        for(int i=startY; i<endY; i++)  {
            loc = (i<<11) + (i<<9) + (startX<<2);

            for(int j=startX; j<endX; j++)   {
                unsigned char &blue = imageData[loc];
                unsigned char &green = imageData[loc+1];
                unsigned char &red = imageData[loc+2];

                if( (red <= ball->getMaxColor().red()+toleranceBand && red >= ball->getMinColor().red()-toleranceBand) &&
                    (green <= ball->getMaxColor().green()+toleranceBand && green >= ball->getMinColor().green()-toleranceBand) &&
                    (blue <= ball->getMaxColor().blue()+toleranceBand && blue >= ball->getMinColor().blue()-toleranceBand) )
                {
                    red = ballSignColor.red();
                    green = ballSignColor.green();
                    blue = ballSignColor.blue();
                }

                if( (red <= robot->getMaxColor().red()+toleranceBand && red >= robot->getMinColor().red()-toleranceBand) &&
                    (green <= robot->getMaxColor().green()+toleranceBand && green >= robot->getMinColor().green()-toleranceBand) &&
                    (blue <= robot->getMaxColor().blue()+toleranceBand && blue >= robot->getMinColor().blue()-toleranceBand) )
                {
                    red = robotSignColor.red();
                    green = robotSignColor.green();
                    blue = robotSignColor.blue();
                }
                loc += 4;
            }
        }
    }
}

/**
 * @brief ImageProcessing::getBoundX
 * @param y
 * @param startX
 * @param endX
 */
void ImageProcessing::getBoundX(int y, int &startX, int &endX)
{
    // y 절편
    int y_intercept;

    /* 직사각형인 case
     * 1. Left Top Y가 Right Top Y와 같고,
     * Left Bottom Y가 Right Bottom Y와 같은 경우.
     * 2. Left Top X가 Left Bottom X와 같고,
     * Right Top X가 Right Bottom X와 같은 경우.
     */
    if( leftTopX == leftBottomX && leftTopY == rightTopY
        && rightTopX == rightBottomX && leftBottomY == rightBottomY )
    {
        startX = leftTopX;
        endX = rightTopX;
        return;
    }

    /* 왼쪽 변이 한개 있는 case = 오른쪽 변이 세개 있는 case
     * 1. Left Top Point가 Right Top Point보다 y값이 작고,
     * Left Bottom Point가 Right Bottom Point보다 y값이 큰 경우.
     */
    if( leftTopY < rightTopY && leftBottomY > rightBottomY )    {
        // y = ax+b  ->  y절편 = y-ax
        y_intercept = leftTopY-(leftTopX*gradientD);
        startX = (y - y_intercept) / gradientD;

        if( y <= rightTopY )  {
            y_intercept = rightTopY-(rightTopX*gradientA);
            endX = (y - y_intercept) / gradientA;
        }
        else if( y <= rightBottomY )  {
            y_intercept = rightTopY-(rightTopX*gradientB);
            endX = (y - y_intercept) / gradientB;
        }
        else    {
            y_intercept = rightBottomY-(rightBottomX*gradientC);
            endX = (y - y_intercept) / gradientC;
        }
    }

    /* 왼쪽 변이 두개 있는 case = 오른쪽 변이 두개 있는 case
     * 1. Left Top Point가 Right Top Point보다 y값이 작고,
     * Left Bottom Point가 Right Bottom Point보다 y값이 작은 경우.
     * 2. Left Top Point가 Right Top Point보다 y값이 크고,
     * Left Bottom Point가 Right Bottom Point보다 y값이 큰 경우.
     */
    if( leftTopY <= rightTopY && leftBottomY <= rightBottomY )    {
        if( y <= leftBottomY )   {
            y_intercept = leftBottomY-(leftBottomX*gradientD);
            startX = (y - y_intercept) / gradientD;
        }
        else    {
            y_intercept = leftBottomY-(leftBottomX*gradientC);
            startX = (y - y_intercept) / gradientC;
        }

        if( y <= rightTopY )   {
            y_intercept = rightTopY-(rightTopX*gradientA);
            endX = (y - y_intercept) / gradientA;
        }
        else    {
            y_intercept = rightTopY-(rightTopX*gradientB);
            endX = (y - y_intercept) / gradientB;
        }
    }
    if( leftTopY > rightTopY && leftBottomY < rightBottomY )    {
        if( y <= leftTopY )  {
            y_intercept = leftTopY-(leftTopX*gradientA);
            startX = (y - y_intercept) / gradientA;
        }
        else    {
            y_intercept = leftTopY-(leftTopX*gradientD);
            startX = (y - y_intercept) / gradientD;
        }

        if( y <= rightBottomY )  {
            y_intercept = rightBottomY-(rightBottomX*gradientB);
            endX = (y - y_intercept) / gradientB;
        }
        else    {
            y_intercept = rightBottomY-(rightBottomX*gradientC);
            endX = (y - y_intercept) / gradientC;
        }
    }

    /* 왼쪽 변이 세개 있는 case = 오른쪽 변이 한개 있는 case
     * 1. Left Top Point가 Right Top Point보다 y값이 크고,
     * Left Bottom Point가 Right Bottom Point보다 y값이 작은 경우.
     */
    if( leftTopY >= rightTopY && leftBottomY <= rightBottomY )    {
        if( y <= leftTopY )  {
            y_intercept = leftTopY-(leftTopX*gradientA);
            startX = (y - y_intercept) / gradientA;
        }
        else if( y <= leftBottomY )  {
            y_intercept = leftTopY-(leftTopX*gradientD);
            startX = (y - y_intercept) / gradientD;
        }
        else    {
            y_intercept = leftBottomY-(leftBottomX*gradientC);
            startX = (y - y_intercept) / gradientC;
        }

        y_intercept = rightTopY-(rightTopX*gradientB);
        endX = (y - y_intercept) / gradientB;
    }
}

/**
 * @brief ImageProcessing::slotDraggedImage
 * @param x
 * @param y
 */
void ImageProcessing::slotDraggedImage(int x, int y)
{
    QColor maskColor = QColor::fromRgb(rawImage.pixel(x, y));

    int red = maskColor.red();
    int green = maskColor.green();
    int blue = maskColor.blue();

    switch(radioState)  {
    case RADIO_BALL:
        ball->updateColor(red, green, blue);
        break;
    case RADIO_ROBOT:
        robot->updateColor(red, green, blue);
        break;
    }
}

/**
 * @brief ImageProcessing::slotResetMaskColor
 */
void ImageProcessing::slotResetMaskColor()
{
    switch(radioState)  {
    case RADIO_BALL:
        ball->resetColor();
        break;
    case RADIO_ROBOT:
        robot->resetColor();
        break;
    }
}

/**
 * @brief ImageProcessing::radioStateChanged
 * @param radioState
 */
void ImageProcessing::radioStateChanged(RadioState radioState)
{
    this->radioState = radioState;
}

/**
 * @brief ImageProcessing::slotBoardAreaReady
 * @param isBoardAreaReady
 */
void ImageProcessing::slotBoardAreaReady(bool isBoardAreaReady)
{
    this->isBoardAreaReady = isBoardAreaReady;
}

/**
 * @brief ImageProcessing::slotMorpologyEnable
 * @param check
 */
void ImageProcessing::slotMorpologyEnable(bool check)
{
    enableMorpology = check;
}

/**
 * @brief ImageProcessing::getPosition
 * @param frameImage
 * @return
 */
void ImageProcessing::getObjectsPosition(QImage *frameImage)
{
    if( !isBoardAreaReady )
        return;    // Board area not ready

    int x;
    int y;
    int maxBall = 0;
    int maxRobot = 0;
    int maxIndex = 0;
    int groupBallNum = 1;
    int groupRobotNum = 1;

    Outline maxBallOutline;
    Outline maxRobotOutline;
    Outline *ballOutline = new Outline[groupBallNum];
    Outline *robotOutline = new Outline[groupRobotNum];
    int *groupBall = new int[groupBallNum];
    int *groupRobot = new int[groupBallNum];

    imageData = frameImage->bits();
    memset(labelBall, 0, sizeof(labelBall));
    memset(labelRobot, 0, sizeof(labelRobot));

    int startX = leftTopPoint.x();
    int startY = leftTopPoint.y();
    int endX = rightBottomPoint.x();
    int endY = rightBottomPoint.y();

    for(int i=startY; i<endY; i++)    {
        for(int j=startX; j<endX; j++)    {
            int loc = i*2560 + j*4;
            QColor *color = new QColor(imageData[loc+2],
                                       imageData[loc+1],
                                       imageData[loc],
                                       255);

            if( *color == ballSignColor && labelBall[i][j] == 0 )
            {
                bool isBallGroupAdjacent = false;

                for(int k=0; k<4; k++)  {
                    x = j + dir[k][1];
                    y = i + dir[k][0];

                    if( labelBall[y][x] > 0 )   {
                        isBallGroupAdjacent = true;
                        labelBall[i][j] = labelBall[y][x];

                        if( maxBall < ++groupBall[labelBall[i][j]-1] )    {
                            maxBall = groupBall[labelBall[i][j]-1];
                            maxIndex = labelBall[i][j]-1;
                            maxBallOutline = ballOutline[maxIndex];
                        }

                        if( x < ballOutline[labelBall[i][j]-1].leftUpSide.x() )
                            ballOutline[labelBall[i][j]-1].leftUpSide.setX(j);
                        if( y < ballOutline[labelBall[i][j]-1].leftUpSide.y() )
                            ballOutline[labelBall[i][j]-1].leftUpSide.setY(i);
                        if( x > ballOutline[labelBall[i][j]-1].rightDownSide.x() )
                            ballOutline[labelBall[i][j]-1].rightDownSide.setX(j);
                        if( y > ballOutline[labelBall[i][j]-1].rightDownSide.y() )
                            ballOutline[labelBall[i][j]-1].rightDownSide.setY(i);

                        break;
                    }
                }
                if( isBallGroupAdjacent )
                    continue;
                else    {
                    labelBall[i][j] = ++groupBallNum;

                    int *tempGroup = new int[groupBallNum];
                    Outline *tempOutline = new Outline[groupBallNum];

                    memcpy(tempGroup, groupBall, sizeof(int)*(groupBallNum-1));
                    memcpy(tempOutline, ballOutline, sizeof(Outline)*(groupBallNum-1));
                    delete groupBall;
                    delete ballOutline;

                    groupBall = tempGroup;
                    ballOutline = tempOutline;

                    groupBall[groupBallNum-1] = 1;
                    ballOutline[groupBallNum-1].leftUpSide.setX(j);
                    ballOutline[groupBallNum-1].leftUpSide.setY(i);
                    ballOutline[groupBallNum-1].rightDownSide.setX(j);
                    ballOutline[groupBallNum-1].rightDownSide.setY(i);
                }
            }

            if( *color == robotSignColor && labelRobot[i][j] == 0 )
            {
                bool isRobotGroupAdjacent = false;

                for(int k=0; k<4; k++)  {
                    x = j + dir[k][1];
                    y = i + dir[k][0];

                    if( labelRobot[y][x] > 0 )   {
                        isRobotGroupAdjacent = true;
                        labelRobot[i][j] = labelRobot[y][x];

                        if( maxRobot < ++groupRobot[labelRobot[i][j]-1] )    {
                            maxRobot = groupRobot[labelRobot[i][j]-1];
                            maxIndex = labelRobot[i][j]-1;
                            maxRobotOutline = robotOutline[maxIndex];
                        }

                        if( x < robotOutline[labelRobot[i][j]-1].leftUpSide.x() )
                            robotOutline[labelRobot[i][j]-1].leftUpSide.setX(j);
                        if( y < robotOutline[labelRobot[i][j]-1].leftUpSide.y() )
                            robotOutline[labelRobot[i][j]-1].leftUpSide.setY(i);
                        if( x > robotOutline[labelRobot[i][j]-1].rightDownSide.x() )
                            robotOutline[labelRobot[i][j]-1].rightDownSide.setX(j);
                        if( y > robotOutline[labelRobot[i][j]-1].rightDownSide.y() )
                            robotOutline[labelRobot[i][j]-1].rightDownSide.setY(i);

                        break;
                    }
                }
                if( isRobotGroupAdjacent )
                    continue;
                else    {
                    labelRobot[i][j] = ++groupRobotNum;

                    int *tempGroup = new int[groupRobotNum];
                    Outline *tempOutline = new Outline[groupRobotNum];

                    memcpy(tempGroup, groupRobot, sizeof(int)*(groupRobotNum-1));
                    memcpy(tempOutline, robotOutline, sizeof(Outline)*(groupRobotNum-1));
                    delete groupRobot;
                    delete robotOutline;

                    groupRobot = tempGroup;
                    robotOutline = tempOutline;

                    groupRobot[groupRobotNum-1] = 1;
                    robotOutline[groupRobotNum-1].leftUpSide.setX(j);
                    robotOutline[groupRobotNum-1].leftUpSide.setY(i);
                    robotOutline[groupRobotNum-1].rightDownSide.setX(j);
                    robotOutline[groupRobotNum-1].rightDownSide.setY(i);
                }
            }
        }
    }

    ballPos.setX((maxBallOutline.leftUpSide.x()
                  + maxBallOutline.rightDownSide.x()) / 2);
    ballPos.setY((maxBallOutline.leftUpSide.y()
                  + maxBallOutline.rightDownSide.y()) / 2);
    robotPos.setX((maxRobotOutline.leftUpSide.x()
                   + maxRobotOutline.rightDownSide.x()) / 2);
    robotPos.setY((maxRobotOutline.leftUpSide.y()
                   + maxRobotOutline.rightDownSide.y()) / 2);
}

/**
 * @brief ImageProcessing::erode
 * @param sourceImage
 */
void ImageProcessing::erode(QImage *sourceImage)
{
    if( !isBoardAreaReady )
        return;

    int end = SCREEN_WIDTH*SCREEN_HEIGHT*4;
    bool check[end];
    memset(check, false, sizeof(check));

    int boundStartY = (leftTopY<rightTopY)
                     ? leftTopY
                     : rightTopY;
    int boundEndY = (leftBottomY>rightBottomY)
                    ? leftBottomY
                    : rightBottomY;

    imageData = sourceImage->bits();
    rawImageData = rawImage.bits();

    for(int i=boundStartY; i<boundEndY; i++)    {
        int boundStartX;
        int boundEndX;

        getBoundX(i, boundStartX, boundEndX);

        if( boundStartX < 0 || boundEndX < 0
            || boundEndX > SCREEN_WIDTH )
            continue;

        for(int j=boundStartX; j<boundEndX; j++)    {
            int loc = i*2560 + j*4;

            unsigned char blue = imageData[loc];
            unsigned char green = imageData[loc+1];
            unsigned char red = imageData[loc+2];

            if( !(blue == ballSignColor.blue()
                  && red == ballSignColor.red()
                  && green == ballSignColor.green()) )
            {
                for(int k=0; k<8; k++)  {

                    int _loc = (i+dir[k][0])*2560 + (j+dir[k][1])*4;

                    unsigned char _blue = imageData[_loc];
                    unsigned char _green = imageData[_loc+1];
                    unsigned char _red = imageData[_loc+2];

                    if( _blue == ballSignColor.blue()
                        && _red == ballSignColor.red()
                        && _green == ballSignColor.green() )
                    {
                        check[_loc] = true;
                    }
                }
            }
        }
    }

    for(int i=boundStartY; i<boundEndY; i++)   {
        int boundStartX;
        int boundEndX;

        getBoundX(i, boundStartX, boundEndX);

        if( boundStartX < 0 || boundEndX < 0
            || boundEndX > SCREEN_WIDTH )
            continue;

        for(int j=boundStartX; j<boundEndX; j++)    {
            int loc = i*2560 + j*4;
            if( check[loc] == true )    {
                imageData[loc] = rawImageData[loc];
                imageData[loc+1] = rawImageData[loc+1];
                imageData[loc+2] = rawImageData[loc+2];
                imageData[loc+3] = rawImageData[loc+3];
            }
        }
    }
}

/**
 * @brief ImageProcessing::dilate
 * @param sourceImage
 */
void ImageProcessing::dilate(QImage *sourceImage)
{
    if( !isBoardAreaReady )
        return;

    int end = SCREEN_WIDTH*SCREEN_HEIGHT*4;
    bool check[end];
    memset(check, false, sizeof(check));

    int boundStartY = (leftTopY<rightTopY)
                     ? leftTopY
                     : rightTopY;
    int boundEndY = (leftBottomY>rightBottomY)
                    ? leftBottomY
                    : rightBottomY;

    imageData = sourceImage->bits();

    for(int i=boundStartY; i<boundEndY; i++)    {
        int boundStartX;
        int boundEndX;

        getBoundX(i, boundStartX, boundEndX);

        if( boundStartX < 0 || boundEndX < 0
            || boundEndX > SCREEN_WIDTH )
            continue;

        for(int j=boundStartX; j<boundEndX; j++)    {
            int loc = i*2560 + j*4;

            unsigned char blue = imageData[loc];
            unsigned char green = imageData[loc+1];
            unsigned char red = imageData[loc+2];

            if( blue == ballSignColor.blue()
                && red == ballSignColor.red()
                && green == ballSignColor.green() )
            {
                for(int k=0; k<8; k++)  {
                    int _loc = (i+dir[k][0])*2560 + (j+dir[k][1])*4;

                    unsigned char _blue = imageData[_loc];
                    unsigned char _green = imageData[_loc+1];
                    unsigned char _red = imageData[_loc+2];

                    if( !(_blue == ballSignColor.blue()
                          && _red == ballSignColor.red()
                          && _green == ballSignColor.green()) ) {
                        check[_loc] = true;
                    }
                }
            }
        }
    }

    for(int i=boundStartY; i<boundEndY; i++)   {
        int boundStartX;
        int boundEndX;

        getBoundX(i, boundStartX, boundEndX);

        if( boundStartX < 0 || boundEndX < 0
            || boundEndX > SCREEN_WIDTH )
            continue;

        for(int j=boundStartX; j<boundEndX; j++)    {
            int loc = i*2560 + j*4;
            if( check[loc] == true )    {
                imageData[loc] = 255;
                imageData[loc+1] = 0;
                imageData[loc+2] = 0;
                imageData[loc+3] = 255;
            }
        }
    }
}

void ImageProcessing::predictCourse(QImage *dstImage)
{
    QPainter painter(dstImage);

    painter.setPen(QColor(Qt::blue));
    painter.drawEllipse(robotPos, 5, 5);
    painter.setPen(QColor(Qt::red));
    painter.drawEllipse(ballPos, 5, 5);

    if( ball->isMoving == true )  {
        // y = ax + b
        int x = ballPos.x();
        int y = ballPos.y();

        QPoint aimPoint;
        int collisionCount = 0;
        double a = ball->gradient;

        while(true) {
            double y_intercept = y - (a * (double)x);
            if( ++collisionCount == 3 )
                break;

            if( ball->direction == NORTH_EAST || ball->direction == NORTH_WEST )    {
                aimPoint.setX((double)(leftTopPoint.y()-y_intercept) / a);

                if( aimPoint.x() > rightBottomPoint.x() )  {

                    aimPoint.setX(rightBottomPoint.x());
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else if( aimPoint.x() < leftTopPoint.x() ) {
                    aimPoint.setX(leftTopPoint.x());
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else    {
                    aimPoint.setY(leftTopPoint.y());
                    painter.drawLine(QPoint(x, y), aimPoint);

                    a *= -1;
                    x = aimPoint.x();
                    y = aimPoint.y();

                    if( ball->direction == NORTH_EAST )
                        ball->direction = SOUTH_EAST;
                    else
                        ball->direction = SOUTH_WEST;
                }
            }
            else if( ball->direction == SOUTH_EAST || ball->direction == SOUTH_WEST )   {
                if( robotSide == RIGHT_SIDE )   {

                }
                aimPoint.setX((double)(rightBottomPoint.y()-y_intercept) / a);

                if( aimPoint.x() > rightBottomPoint.x() )  {
                    aimPoint.setX(rightBottomPoint.x());
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else if( aimPoint.x() < leftTopPoint.x() ) {
                    aimPoint.setX(leftTopPoint.x());
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else    {
                    aimPoint.setY(rightBottomPoint.y());
                    painter.drawLine(QPoint(x, y), aimPoint);

                    a *= -1;
                    x = aimPoint.x();
                    y = aimPoint.y();

                    if( ball->direction == SOUTH_EAST )
                        ball->direction = NORTH_EAST;
                    else
                        ball->direction = NORTH_WEST;
                }
            }

            if( ball->direction == NORTH_EAST || ball->direction == SOUTH_EAST )    {
                int predictX = rightBottomPoint.x() - DEFENCE_DISTANCE;
                int predictY = (a*predictX) + y_intercept;
                ball->predictPoint = QPoint(predictX, predictY);
            }
            else    {
                int predictX = rightBottomPoint.x() + DEFENCE_DISTANCE;
                int predictY = (a*predictX) + y_intercept;
                ball->predictPoint = QPoint(predictX, predictY);
            }
        }
    }
}

/**
 * @brief ImageProcessing::slotMouseClick
 * @param mousePoint
 */
void ImageProcessing::slotMouseClick(QPoint mousePoint)
{
    isMouseClick = true;

    if( isRectangleBoardMode == true )  {
        ball->startPoint = leftTopPoint = mousePoint;

        isRectangleGrabbing = true;
    }
}

/**
 * @brief ImageProcessing::slotMouseMove
 * @param mousePoint
 */
void ImageProcessing::slotMouseMove(QPoint mousePoint)
{
    mousePos = mousePoint;

    if( isMouseClick )  {
        if( isRectangleBoardMode == true )  {
            rightBottomPoint = mousePoint;
        }

        if( isBoardAreaReady == true )
            slotDraggedImage(mousePos.x(), mousePos.y());
    }
}

/**
 * @brief ImageProcessing::slotMouseRelease
 * @param mousePoint
 */
void ImageProcessing::slotMouseRelease(QPoint mousePoint)
{
    isMouseClick = false;

    if( isRectangleBoardMode == true )  {
        isRectangleBoardMode = false;
        isRectangleGrabbing = false;
        isBoardAreaReady = true;

        rightBottomPoint = mousePoint;

        int unit = 23;
        robot->unitX = (rightBottomPoint.x()-leftTopPoint.x()) / unit;
        robot->unitY = (rightBottomPoint.y()-leftTopPoint.y()) / unit;
    }
}

/**
 * @brief ImageProcessing::slotSetRectangleBoardArea
 */
void ImageProcessing::slotSetRectangleBoardArea()
{
    leftTopPoint = QPoint(0, 0);
    rightBottomPoint = QPoint(0, 0);

    isRectangleBoardMode = true;
}

/**
 * @brief ImageProcessing::slotRobotSideChanged
 * @param robotSide
 */
void ImageProcessing::slotRobotSideChanged(int robotSide)
{
    this->robotSide = (RobotSide)robotSide;
}

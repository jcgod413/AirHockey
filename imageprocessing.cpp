#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent) :
    QObject(parent),
    ball(new Ball),
    ballColor(new QColor(0, 0, 255)),
    ballPos(0, 0),
    mousePos(0, 0),
    predictPoint(0, 0),
    isMouseClick(false),
    isBoardAreaReady(false),
    enableMorpology(false),
    isRectangleReady(false),
    isRectangleBoardMode(false),
    isRectangleGrabbing(false),
    redMax(0),
    greenMax(0),
    blueMax(0),
    redMin(255),
    greenMin(255),
    blueMin(255),
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
//    initImageProcessing();
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

    // get ball position
    ball->find(ballPos = getBallPosition(&resultImage));
    ball->updateInfo();
    emit signalFindBall(ballPos);

    // predict
    predictCourse(&resultImage);

    // tracking
    ballTracking();

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

    if( robotDirection == LEFT_SIDE )   {
        // 예상 도달 지점 잡기
    }
    else if( robotDirection == RIGHT_SIDE ) {

    }
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
        for(int i=leftTopPoint.y(); i<rightBottomPoint.y(); i++)  {
            for(int j=leftTopPoint.x(); j<rightBottomPoint.x(); j++)   {
                int loc = i*2560 + j*4;

                unsigned char &blue = imageData[loc];
                unsigned char &green = imageData[loc+1];
                unsigned char &red = imageData[loc+2];

                if( (red <= redMax+toleranceBand && red >= redMin-toleranceBand) &&
                    (green <= greenMax+toleranceBand && green >= greenMin-toleranceBand) &&
                    (blue <= blueMax+toleranceBand && blue >= blueMin-toleranceBand) )
                {
                    red = ballColor->red();
                    green = ballColor->green();
                    blue = ballColor->blue();
                }
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

    redMax   = (redMax<red) ? red : redMax;
    redMin   = (redMin>red) ? red : redMin;
    greenMax = (greenMax<green) ? green : greenMax;
    greenMin = (greenMin>green) ? green : greenMin;
    blueMax  = (blueMax<blue) ? blue : blueMax;
    blueMin  = (blueMin>blue) ? blue : blueMin;
}

/**
 * @brief ImageProcessing::slotResetMaskColor
 */
void ImageProcessing::slotResetMaskColor()
{
    redMax = greenMax = blueMax = 0;
    redMin = greenMin = blueMin = 255;
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
 * @brief ImageProcessing::getBallPosition
 * @param frameImage
 * @return
 */
QPoint ImageProcessing::getBallPosition(QImage *frameImage)
{
    if( !isBoardAreaReady )
        return QPoint(0, 0);    // Board area not ready

    int x;
    int y;
    int max = 0;
    int maxIndex = 0;
    int groupNum = 1;

    Outline maxOutline;
    Outline *outline = new Outline[groupNum];
    int *group = new int[groupNum];

    imageData = frameImage->bits();
    memset(label, 0, sizeof(label));

    for(int i=leftTopPoint.y(); i<rightBottomPoint.y(); i++)    {
        for(int j=leftTopPoint.x(); j<rightBottomPoint.x(); j++)    {
            int loc = i*2560 + j*4;
            QColor *color = new QColor(imageData[loc+2],
                                       imageData[loc+1],
                                       imageData[loc],
                                       255);

            if( color->operator ==(*ballColor) && label[i][j] == 0 )
            {
                bool isGroupAdjacent = false;

                for(int k=0; k<4; k++)  {
                    x = j + dir[k][1];
                    y = i + dir[k][0];

                    if( label[y][x] > 0 )   {
                        isGroupAdjacent = true;
                        label[i][j] = label[y][x];

                        if( max < ++group[label[i][j]-1] )    {
                            max = group[label[i][j]-1];
                            maxIndex = label[i][j]-1;
                            maxOutline = outline[maxIndex];
                        }

                        if( x < outline[label[i][j]-1].leftUpSide.x() )
                            outline[label[i][j]-1].leftUpSide.setX(j);
                        if( y < outline[label[i][j]-1].leftUpSide.y() )
                            outline[label[i][j]-1].leftUpSide.setY(i);
                        if( x > outline[label[i][j]-1].rightDownSide.x() )
                            outline[label[i][j]-1].rightDownSide.setX(j);
                        if( y > outline[label[i][j]-1].rightDownSide.y() )
                            outline[label[i][j]-1].rightDownSide.setY(i);

                        break;
                    }
                }
                if( isGroupAdjacent )
                    continue;
                else    {
                    label[i][j] = ++groupNum;

                    int *tempGroup = new int[groupNum];
                    Outline *tempOutline = new Outline[groupNum];

                    memcpy(tempGroup, group, sizeof(int)*(groupNum-1));
                    memcpy(tempOutline, outline, sizeof(Outline)*(groupNum-1));
                    delete group;
                    delete outline;

                    group = tempGroup;
                    outline = tempOutline;

                    group[groupNum-1] = 1;
                    outline[groupNum-1].leftUpSide.setX(j);
                    outline[groupNum-1].leftUpSide.setY(i);
                    outline[groupNum-1].rightDownSide.setX(j);
                    outline[groupNum-1].rightDownSide.setY(i);
                }
            }

            delete color;
        }
    }

    return QPoint((maxOutline.leftUpSide.x() + maxOutline.rightDownSide.x()) / 2,
                  (maxOutline.leftUpSide.y() + maxOutline.rightDownSide.y()) / 2);
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

            if( !(blue == ballColor->blue()
                  && red == ballColor->red()
                  && green == ballColor->green()) )
            {
                for(int k=0; k<8; k++)  {

                    int _loc = (i+dir[k][0])*2560 + (j+dir[k][1])*4;

                    unsigned char _blue = imageData[_loc];
                    unsigned char _green = imageData[_loc+1];
                    unsigned char _red = imageData[_loc+2];

                    if( _blue == ballColor->blue()
                        && _red == ballColor->red()
                        && _green == ballColor->green() )
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

            if( blue == ballColor->blue()
                && red == ballColor->red()
                && green == ballColor->green() )
            {
                for(int k=0; k<8; k++)  {
                    int _loc = (i+dir[k][0])*2560 + (j+dir[k][1])*4;

                    unsigned char _blue = imageData[_loc];
                    unsigned char _green = imageData[_loc+1];
                    unsigned char _red = imageData[_loc+2];

                    if( !(_blue == ballColor->blue()
                          && _red == ballColor->red()
                          && _green == ballColor->green()) ) {
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
    QPixmap pix = QPixmap::fromImage(*dstImage);
    QPainter painter(&pix);

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
        }
    }

    *dstImage = pix.toImage();
}

/**
 * @brief ImageProcessing::slotScreenClick
 * @param mousePoint
 */
void ImageProcessing::slotScreenClick(QPoint mousePoint)
{
    isMouseClick = true;

    if( isRectangleBoardMode == true )  {
        leftTopPoint = mousePoint;

        isRectangleGrabbing = true;
    }
}

/**
 * @brief ImageProcessing::slotScreenMove
 * @param mousePoint
 */
void ImageProcessing::slotScreenMove(QPoint mousePoint)
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
 * @brief ImageProcessing::slotScreenRelease
 */
void ImageProcessing::slotScreenRelease(QPoint mousePoint)
{
    isMouseClick = false;

    if( isRectangleBoardMode == true )  {
        isRectangleBoardMode = false;
        isRectangleGrabbing = false;
        isBoardAreaReady = true;

        rightBottomPoint = mousePoint;
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
 * @brief ImageProcessing::slotRobotDirectionChanged
 * @param robotDirection
 */
void ImageProcessing::slotRobotDirectionChanged(int robotDirection)
{
    this->robotDirection = (RobotDirection)robotDirection;
}

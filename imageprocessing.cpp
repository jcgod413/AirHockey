#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent) :
    QObject(parent),
    ball(new Ball),
    ballColor(new QColor(0, 0, 255)),
    isBoardAreaReady(false),
    enableMorpology(false),
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

}

/**
 * @brief ImageProcessing::imageProcess
 * @param rawImage
 * @return
 */
QImage ImageProcessing::imageProcess(QImage *rawImage)
{
    this->rawImage = *rawImage;
    QImage resultImage;

    // threshold
    getThresholdImage(&resultImage);

    // morpology
    if( enableMorpology )   {
        for(int i=0; i<erodeNum; i++)
            erode(&resultImage);
        for(int i=0; i<dilateNum; i++)
            dilate(&resultImage);
    }

    // get ball position
    ball->find(ballPos = getBallPosition(&resultImage));
    ball->updateInfo();

    // predict
    predictCourse(&resultImage);

    // tracking

    return resultImage;
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

    if( isBoardAreaReady )  {
        for(int i=boundStartY; i<boundEndY; i++)    {
            int boundStartX;
            int boundEndX;

            getBoundX(i, boundStartX, boundEndX);

            if( boundStartX < 0 || boundEndX < 0
                || boundEndX > SCREEN_WIDTH )
                continue;

            for(int j=boundStartX; j<boundEndX; j++)    {
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

    qDebug("%d %d %d", red, green, blue);
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
 * @brief ImageProcessing::slotBoardAreaPoint
 * @param boardAreaClick
 * @param x
 * @param y
 */
void ImageProcessing::slotBoardAreaPoint(int boardAreaClick, int x, int y)
{
    switch(boardAreaClick)  {
    case LEFT_TOP:
        leftTopX = x;
        leftTopY = y;
        break;
    case RIGHT_TOP:
        rightTopX = x;
        rightTopY = y;
        break;
    case RIGHT_BOTTOM:
        rightBottomX = x;
        rightBottomY = y;
        break;
    case LEFT_BOTTOM:
        leftBottomX = x;
        leftBottomY = y;

        // Left Top Point와 Right Top Point의 기울기
        gradientA = (double)(rightTopY-leftTopY)
                   / (double)(rightTopX-leftTopX);
        // Right Top Point와 Right Bottom Point의 기울기
        gradientB = (double)(rightBottomY-rightTopY)
                   / (double)(rightBottomX-rightTopX);
        // Right Bottom Point와 Left Bottom Point의 기울기
        gradientC = (double)(leftBottomY-rightBottomY)
                   / (double)(leftBottomX-rightBottomX);
        // Left Bottom Point와 Left Top Point의 기울기
        gradientD = (double)(leftBottomY-leftTopY)
                   / (double)(leftBottomX-leftTopX);

        emit signalRectangleReady(true);
        emit signalBoardArea(false);

        break;
    case RESET_BOARD_AREA:   // 경기장 영역 클릭
        leftTopX = leftTopY = leftBottomX = leftBottomY = 0;
        rightTopX = rightTopY = rightBottomX = rightBottomY = 0;
        isBoardAreaReady = false;
        break;
    }
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

    int boundStartY = (leftTopY < rightTopY)
                      ? leftTopY
                      : rightTopY;
    int boundEndY = (leftBottomY > rightBottomY)
                    ? leftBottomY
                    : rightBottomY;

    for(int i=boundStartY; i<boundEndY; i++)    {
        int boundStartX;
        int boundEndX;

        getBoundX(i, boundStartX, boundEndX);

        if( boundStartX < 0 || boundEndX < 0
            || boundEndX > SCREEN_WIDTH )
            continue;

        for(int j=boundStartX; j<boundEndX; j++)    {
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
            qDebug("%d", ball->direction);

            if( ball->direction == NORTH_EAST || ball->direction == NORTH_WEST )    {
                aimPoint.setX((double)(rightTopY-y_intercept) / a);

                if( aimPoint.x() > rightTopX )  {
                    aimPoint.setX(rightTopX);
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else if( aimPoint.x() < leftTopX ) {
                    aimPoint.setX(leftTopX);
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else    {
                    aimPoint.setY(rightTopY);
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
                aimPoint.setX((double)(rightBottomY-y_intercept) / a);

                if( aimPoint.x() > rightTopX )  {
                    aimPoint.setX(rightTopX);
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else if( aimPoint.x() < leftTopX ) {
                    aimPoint.setX(leftTopX);
                    aimPoint.setY(a * aimPoint.x() + y_intercept);
                    painter.drawLine(QPoint(x, y), aimPoint);
                    break;
                }
                else    {
                    aimPoint.setY(rightBottomY);
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
//        qDebug("%d  %d", aimPoint.x(), aimPoint.y());
    }

    *dstImage = pix.toImage();
}

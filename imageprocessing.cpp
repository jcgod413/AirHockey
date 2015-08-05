#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent) :
    QObject(parent),
    ballColor(new QColor(0, 0, 255)),
    isBoardAreaReady(false),
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
    erodeNum(0),
    dilateNum(0)
{

}

ImageProcessing::~ImageProcessing()
{

}

void ImageProcessing::loadRawImage(QImage rawImage)
{
    this->rawImage = rawImage;
}

QImage ImageProcessing::getThresholdImage()
{
    QElapsedTimer timer;
    timer.start();

    int boundStartY = (leftTopY<rightTopY)
                     ? leftTopY
                     : rightTopY;
    int boundEndY = (leftBottomY>rightBottomY)
                    ? leftBottomY
                    : rightBottomY;

    QImage thresholdImage = rawImage;

    unsigned char *imageData = thresholdImage.bits();

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

    for(int i=0; i<erodeNum; i++)
        erode(thresholdImage);
    for(int i=0; i<dilateNum; i++)
        dilate(thresholdImage);

    QPoint ballPosition = getBallPosition(&thresholdImage);
    emit signalFindBall(ballPosition);

//    qDebug("%d", timer.elapsed());
    return thresholdImage;
}

void ImageProcessing::getBoundX(int y, int &startX, int &endX)
{
    // y 절편
    int y_intercept;

    /* 왼쪽 변이 한개 있는 case = 오른쪽 변이 세개 있는 case
     * 1. Left Top Point가 Right Top Point보다 y값이 작고,
     * Left Bottom Point가 Right Bottom Point보다 y값이 큰 경우.
     */
    if( leftTopY <= rightTopY && leftBottomY >= rightBottomY )    {
        // y = ax+b  ->  y절편 = y-ax
        y_intercept = leftTopY-(leftTopX*gradientD);
        startX = (y - y_intercept) / gradientD;

        if( y < rightTopY )  {
            y_intercept = rightTopY-(rightTopX*gradientA);
            endX = (y - y_intercept) / gradientA;
        }
        else if( y < rightBottomY )  {
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
    if( leftTopY < rightTopY && leftBottomY < rightBottomY )    {
        if( y < leftBottomY )   {
            y_intercept = leftBottomY-(leftBottomX*gradientD);
            startX = (y - y_intercept) / gradientD;
        }
        else    {
            y_intercept = leftBottomY-(leftBottomX*gradientC);
            startX = (y - y_intercept) / gradientC;
        }

        if( y < rightTopY )   {
            y_intercept = rightTopY-(rightTopX*gradientA);
            endX = (y - y_intercept) / gradientA;
        }
        else    {
            y_intercept = rightTopY-(rightTopX*gradientB);
            endX = (y - y_intercept) / gradientB;
        }
    }
    if( leftTopY > rightTopY && leftBottomY > rightBottomY )    {
        if( y < leftTopY )  {
            y_intercept = leftTopY-(leftTopX*gradientA);
            startX = (y - y_intercept) / gradientA;
        }
        else    {
            y_intercept = leftTopY-(leftTopX*gradientD);
            startX = (y - y_intercept) / gradientD;
        }

        if( y < rightBottomY )  {
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
    if( leftTopY > rightTopY && leftBottomY < rightBottomY )    {
        if( y < leftTopY )  {
            y_intercept = leftTopY-(leftTopX*gradientA);
            startX = (y - y_intercept) / gradientA;
        }
        else if( y < leftBottomY )  {
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

void ImageProcessing::slotResetMaskColor()
{
    redMax = greenMax = blueMax = 0;
    redMin = greenMin = blueMin = 255;
}

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

        isBoardAreaReady = true;

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


void ImageProcessing::slotErodeNumChanged(int _erodeNum)
{
    erodeNum = _erodeNum;
}

void ImageProcessing::slotDilateNumChanged(int _dilateNum)
{
    dilateNum = _dilateNum;
}

void ImageProcessing::erode(QImage &sourceImage)
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

    unsigned char *imageData = sourceImage.bits();
    unsigned char *rawImageData = rawImage.bits();

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

void ImageProcessing::dilate(QImage &sourceImage)
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

    unsigned char *imageData = sourceImage.bits();

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


QPoint ImageProcessing::getBallPosition(QImage *frameImage)
{
    if( !isBoardAreaReady )
        return QPoint(0, 0);    // Board area not ready

    int max = 0;
    int groupNum = 1;
    QPoint minPoint;
    QPoint maxPoint;

    unsigned char *imageData = frameImage->bits();
    memset(label, 0, sizeof(label));

    int boundStartY = (leftTopY<rightTopY)
                     ? leftTopY
                     : rightTopY;
    int boundEndY = (leftBottomY>rightBottomY)
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

            if( color->blue() == ballColor->blue()
                && label[i][j] == 0 )
            {
                QPoint _minPoint;
                QPoint _maxPoint;
                groupCnt=0;

                searchGroup(groupNum++, QPoint(j, i),
                            &_minPoint, &_maxPoint, imageData);

                if( groupCnt > max ) {
                    max = groupCnt;
                    maxPoint = _maxPoint;
                    minPoint = _minPoint;
                }
            }
        }
    }
//    qDebug("%d", max);

    return QPoint((minPoint.x()+maxPoint.x())/2,
                  (minPoint.y()+maxPoint.y())/2);
}

void ImageProcessing::searchGroup(int groupNum,
                                  QPoint point,
                                  QPoint *minPoint,
                                  QPoint *maxPoint,
                                  unsigned char *imageData)
{
    for(int i=0; i<8; i++)  {
        int x = point.x() + dir[i][0];
        int y = point.y() + dir[i][1];

        int loc = y*2560 + x*4;
        QColor *color = new QColor(imageData[loc+2],
                                    imageData[loc+1],
                                    imageData[loc],
                                    255);

        if( color->blue() == ballColor->blue() && label[y][x] == 0 )  {
            label[y][x] = groupNum;

            if( x > maxPoint->x() )    maxPoint->setX(x);
            if( x < minPoint->x() )    minPoint->setX(x);
            if( y > maxPoint->y() )    maxPoint->setY(y);
            if( y > minPoint->y() )    minPoint->setY(y);

            groupCnt++;
            searchGroup(groupNum, QPoint(x, y),
                        minPoint, maxPoint, imageData);
        }
    }
}


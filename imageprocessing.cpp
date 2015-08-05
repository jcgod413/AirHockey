#include "imageprocessing.h"

ImageProcessing::ImageProcessing(QObject *parent) :
    QObject(parent),
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
    isBoardAreaReady(false)
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

    int toleranceBand = 5;

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
                    red = 0;
                    green = 0;
                    blue = 255;
                }
            }
        }
    }

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


void ImageProcessing::erodeImage(QImage &sourceImage)
{
    unsigned char *resData = sourceImage.bits();

    int end = SCREEN_WIDTH*SCREEN_HEIGHT*4;
    bool check[end/4];

    for(int i=0; i<end; i+=4)    {
        if( !(resData[i]==255 && resData[i+1]==0 && resData[i+2]==0) ) {
            for(int k=0; k<8; k++)  {   //
                if( (i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4)) >= 0 &&
                    (i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4)) < end &&
                    resData[(i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4))] == 255 )
                    resData[(i + (dir[k][0]*SCREEN_WIDTH) + (dir[k][1]*4))] = 1;
            }
        }
    }

    for(int i=0; i<end; i+=4)   {
        if( resData[i] == 1 )   {
            resData[i] = 0;
            resData[i+1] = 0;
            resData[i+2] = 0;
        }
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

/*
void ImageProcessing::Dilate()
{
    unsigned char *resData = resultImage.bits();
//    int end = WIDTH*HEIGHT*4;

    for(int i=0; i<HEIGHT; i++) {
        for(int j=0; j<WIDTH; j++)  {
            if( resData[(i*WIDTH + j)*4] == 255 ) {
                for(int k=0; k<8; k++)  {
                    if( ((i+dir[k][0])*WIDTH + j+dir[k][1])*4 >= 0 &&
                        ((i+dir[k][0])*WIDTH + j+dir[k][1])*4 < end &&
                        resData[((i+dir[k][0])*WIDTH + j+dir[k][1])*4 ] == 0 )
                        resData[ ((i+dir[k][0])*WIDTH + j+dir[k][1])*4 ] = 1;
                }
            }
        }
    }
    for(int i=0; i<end; i+=4)   {
        if( resData[i] == 1 )   {
            resData[i] = 255;
            resData[i+1] = 255;
            resData[i+2] = 255;
        }
    }
}
*/

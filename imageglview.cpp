#include "imageglview.h"

/**
 * @brief ImageGLView::ImageGLView
 */
ImageGLView::ImageGLView(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    isMouseClicked(false),
    isBoardArea(false),
    isRectangleReady(false),
    leftTopX(0),
    leftTopY(0),
    leftBottomX(0),
    leftBottomY(0),
    rightTopX(0),
    rightTopY(0),
    rightBottomX(0),
    rightBottomY(0),
    boardAreaClick(0),
    redMax(0),
    redMin(255),
    greenMax(0),
    greenMin(255),
    blueMax(0),
    blueMin(255)
{
    setMouseTracking(true);
}

/**
 * @brief ImageGLView::~ImageGLView
 */
ImageGLView::~ImageGLView()
{

}

/**
 * @brief ImageGLView::slotImageLoad
 * @param image
 */
void ImageGLView::slotImageLoad(QPixmap image)
{
    frameImage = image;
    this->repaint();
}

/**
 * @brief ImageGLView::paintEvent
 * @param event
 */
void ImageGLView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if( frameImage.isNull() )   return;

    QPainter painter;

    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painterWidth  = painter.window().width();
    painterHeight = painter.window().height();

    painter.fillRect(0, 0, painterWidth, painterHeight, Qt::black);

    if( painterWidth/4*3 < painterHeight )  {
        startX = 0;
        endX   = painterWidth;
        startY = (painterHeight-(painterWidth/4*3))/2;
        endY   = painterHeight-startY;
    }
    else    {
        startX = (painterWidth-painterHeight/3*4)/2;
        endX   = painterWidth-startX;
        startY = 0;
        endY   = painterHeight;
    }


    QImage image = frameImage.toImage();
    QRect rectDrawArea(startX, startY, endX-startX, endY-startY);

    int len = 640*480*4;
    unsigned char *imageData = image.bits();
    int toleranceBand = 5;

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

        
        for(int j=boundStartX; j<boundEndX; j++)   {
            QColor maskColor = QColor::fromRgb(image.pixel(j, i));
            int red   = maskColor.red();
            int green = maskColor.green();
            int blue  = maskColor.blue();
            
            if( (red <= redMax+toleranceBand && red >= redMin-toleranceBand) &&
                (green <= greenMax+toleranceBand && green >= greenMin-toleranceBand) &&
                (blue <= blueMax+toleranceBand && blue >= blueMin-toleranceBand) )
            {
                maskColor.setRgb(255, 0, 0, 255);
                image.setPixel(j, i, maskColor.value());
            }
        }
    }

    painter.drawImage(rectDrawArea, image);

    if( !isRectangleReady ) {
        if( isBoardArea )    {
            painter.drawEllipse(QPoint(mousePosX, mousePosY), 5, 5);
        }

        switch( boardAreaClick )    {
        case RIGHT_TOP :
            painter.drawLine(QPoint(leftTopX, leftTopY),
                             QPoint(mousePosX, mousePosY));
            break;
        case RIGHT_BOTTOM :
            painter.drawLine(QPoint(leftTopX, leftTopY),
                             QPoint(rightTopX, rightTopY));
            painter.drawLine(QPoint(rightTopX, rightTopY),
                             QPoint(mousePosX, mousePosY));
            break;
        case LEFT_BOTTOM :
            painter.drawLine(QPoint(leftTopX, leftTopY),
                             QPoint(rightTopX, rightTopY));
            painter.drawLine(QPoint(rightTopX, rightTopY),
                             QPoint(rightBottomX, rightBottomY));
            painter.drawLine(QPoint(leftTopX, leftTopY),
                             QPoint(mousePosX, mousePosY));
            painter.drawLine(QPoint(rightBottomX, rightBottomY),
                             QPoint(mousePosX, mousePosY));
            break;
        }
    }
    else    {
        painter.drawLine(QPoint(leftTopX, leftTopY),
                         QPoint(rightTopX, rightTopY));
        painter.drawLine(QPoint(leftTopX, leftTopY),
                         QPoint(leftBottomX, leftBottomY));
        painter.drawLine(QPoint(leftBottomX, leftBottomY),
                         QPoint(rightBottomX, rightBottomY));
        painter.drawLine(QPoint(rightTopX, rightTopY),
                         QPoint(rightBottomX, rightBottomY));
    }
    //            painter.drawEllipse(QPoint((loc%2560)/4, loc/2560), 10, 10);
}

/**
 * @brief ImageGLView::mousePressEvent
 * @param event
 */
void ImageGLView::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    int x = event->x();
    int y = event->y();

    isMouseClicked = true;

    if( isBoardArea )   {
        switch(boardAreaClick)  {
        case LEFT_TOP:
            leftTopX = x;
            leftTopY = y;
            break;
        case RIGHT_TOP:
            rightTopX = x;
            rightTopY = y;
            break;
        case LEFT_BOTTOM:
            leftBottomX = x;
            leftBottomY = y;
            break;
        case RIGHT_BOTTOM:
            rightBottomX = x;
            rightBottomY = y;
            break;
        }

        if( ++boardAreaClick == END )   {
            isRectangleReady = true;
            isBoardArea = false;

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

            qDebug("%f", gradientD);
        }
    }
}

/**
 * @brief ImageGLView::mouseReleaseEvent
 * @param event
 */
void ImageGLView::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    isMouseClicked = false;
}

/**
 * @brief ImageGLView::mouseMoveEvent
 * @param event
 */
void ImageGLView::mouseMoveEvent(QMouseEvent *event)
{
    if( event->x() < 0 || event->y() < 0 )
        return;

    mousePosX = event->x();
    mousePosY = event->y();

    if( isMouseClicked )    {
        QColor maskColor = QColor::fromRgb(frameImage.toImage().pixel(mousePosX, mousePosY));

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

}

/**
 * @brief ImageGLView::slotResetColor
 */
void ImageGLView::slotResetColor()
{
    isMouseClicked = false;

    redMax = greenMax = blueMax = 0;
    redMin = greenMin = blueMin = 255;
}

/**
 * @brief ImageGLView::slotSetBoardArea
 */
void ImageGLView::slotSetBoardArea()
{
    isBoardArea = true;
    isRectangleReady = false;

    this->hasFocus();

    boardAreaClick = 0;

    leftTopX = leftTopY = leftBottomX = leftBottomY = 0;
    rightTopX = rightTopY = rightBottomX = rightBottomY = 0;
}


/**
 * @brief ImageGLView::getBoundX
 * @param y
 * @param startX
 * @param endX
 */
void ImageGLView::getBoundX(int y, int &startX, int &endX)
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

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

    for(int i=0; i<len; i+=4)   {
        int blue = imageData[i+0];
        int green = imageData[i+1];
        int red = imageData[i+2];

        if( (red <= redMax+toleranceBand && red >= redMin-toleranceBand) &&
            (green <= greenMax+toleranceBand && green >= greenMin-toleranceBand) &&
            (blue <= blueMax+toleranceBand && blue >= blueMin-toleranceBand) )
        {
            imageData[i+0] = 0;
            imageData[i+1] = 0;
            imageData[i+2] = 255;
        }
    }

    painter.drawImage(rectDrawArea, image);

    if( isRectangleReady )  {
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
        qDebug("%d    %d %d", boardAreaClick, x, y);
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
    if( isMouseClicked )    {
        mousePosX = event->x();
        mousePosY = event->y();

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

    qDebug("%d %d    %d %d    %d %d", redMax, redMin, greenMax, greenMin, blueMax, blueMin);
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

    boardAreaClick = 0;
}

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
    blueMin(255),
    erodeNum(0),
    dilateNum(0)
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
void ImageGLView::imageLoad(QImage image)
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


//    QImage image = frameImage.toImage();
    QRect rectDrawArea(startX, startY, endX-startX, endY-startY);

//    int len = 640*480*4;
//    unsigned char *imageData = image.bits();

    painter.drawImage(rectDrawArea, frameImage);

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
        }

        emit signalBoardAreaPoint(boardAreaClick++, x, y);
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
    mousePosX = event->x();
    mousePosY = event->y();

    if( mousePosX < 0 || mousePosY < 0
        || mousePosX > SCREEN_WIDTH
        || mousePosY > SCREEN_HEIGHT )
        return;


    if( isMouseClicked )    {
        emit signalDraggedImage(mousePosX, mousePosY);
    }
}

/**
 * @brief ImageGLView::slotSetBoardArea
 */
void ImageGLView::slotSetBoardArea()
{
    isBoardArea = true;
    isRectangleReady = false;

    this->hasFocus();

    boardAreaClick = LEFT_TOP;

    emit signalBoardAreaPoint(RESET_BOARD_AREA, 0, 0);
}



/**
 * @brief ImageGLView::slotErodeNumChanged
 * @param _erodeNum
 */
void ImageGLView::slotErodeNumChanged(int _erodeNum)
{
    erodeNum = _erodeNum;
}

/**
 * @brief ImageGLView::slotDilateNumChanged
 * @param _dilateNum
 */
void ImageGLView::slotDilateNumChanged(int _dilateNum)
{
    dilateNum = _dilateNum;
}


void ImageGLView::slotBoardArea(bool _isBoardArea)
{
    isBoardArea = _isBoardArea;
}


void ImageGLView::slotRectangleReady(bool _isRectangleReady)
{
    isRectangleReady = _isRectangleReady;
}

#include "imageglview.h"

/**
 * @brief ImageGLView::ImageGLView
 */
ImageGLView::ImageGLView(QWidget *parent) :
    QGLWidget(QGLFormat(QGL::SampleBuffers), parent),
    isMouseClicked(false),
    isBoardArea(false),
    isRectangleReady(false),
    isRectangleBoardArea(false),
    isRectangleBoardClick(false),
    ballFound(false),
    isBallMoving(false),
    ballPos(0,0),
    ballGradient(0.0),
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

    QRect rectDrawArea(startX, startY, endX-startX, endY-startY);
    painter.drawImage(rectDrawArea, frameImage);

    if( isBoardArea || !isRectangleReady )    {
        painter.drawEllipse(QPoint(mousePosX, mousePosY), 3, 3);
    }

    if( isRectangleReady || (isRectangleBoardArea && isRectangleBoardClick) )  {
        painter.drawLine(QPoint(leftTopX, leftTopY),
                         QPoint(rightTopX, rightTopY));
        painter.drawLine(QPoint(leftTopX, leftTopY),
                         QPoint(leftBottomX, leftBottomY));
        painter.drawLine(QPoint(leftBottomX, leftBottomY),
                         QPoint(rightBottomX, rightBottomY));
        painter.drawLine(QPoint(rightTopX, rightTopY),
                         QPoint(rightBottomX, rightBottomY));
    }
    else    {
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
}

/**
 * @brief ImageGLView::mousePressEvent
 * @param event
 */
void ImageGLView::mousePressEvent(QMouseEvent *event)
{
    this->hasFocus();

    emit signalMouseLeftClick(QPoint(event->x(), event->y()));
}

/**
 * @brief ImageGLView::mouseReleaseEvent
 * @param event
 */
void ImageGLView::mouseReleaseEvent(QMouseEvent *event)
{
    emit signalMouseRelease(QPoint(event->x(), event->y()));
}

/**
 * @brief ImageGLView::mouseMoveEvent
 * @param event
 */
void ImageGLView::mouseMoveEvent(QMouseEvent *event)
{
    emit signalMouseMove(QPoint(event->x(), event->y()));
}


/**
 * @brief ImageGLView::slotBoardArea
 * @param isBoardArea
 */
void ImageGLView::slotBoardArea(bool isBoardArea)
{
    this->isBoardArea = isBoardArea;
}

/**
 * @brief ImageGLView::slotRectangleReady
 * @param isRectangleReady
 */
void ImageGLView::slotRectangleReady(bool isRectangleReady)
{
    this->isRectangleReady = isRectangleReady;
}

/**
 * @brief ImageGLView::slotRobotDirectionChanged
 * @param robotDirection
 */
void ImageGLView::slotRobotDirectionChanged(int robotDirection)
{
    this->robotDirection = (RobotDirection)robotDirection;
}

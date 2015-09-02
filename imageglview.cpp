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
        painter.drawEllipse(QPoint(mousePosX, mousePosY), 5, 5);
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

    if( ballFound == true ) {
        painter.setPen(QColor(Qt::red));
        painter.drawEllipse(ballPos, 5, 5);

        if( isBallMoving == true )  {
            // y = ax + b
            int x = ballPos.x();
            int y = ballPos.y();

            QPoint aimPoint;
            int collisionCount = 0;
            double a = ballGradient;

            while(true) {
                double y_intercept = y - (a * (double)x);
                if( ++collisionCount == 3 )
                    break;

                if( ballDirection == NORTH_EAST || ballDirection == NORTH_WEST )    {
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

                        if( ballDirection == NORTH_EAST )
                            ballDirection = SOUTH_EAST;
                        else
                            ballDirection = SOUTH_WEST;
                    }
                }
                else if( ballDirection == SOUTH_EAST || ballDirection == SOUTH_WEST )   {
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

                        if( ballDirection == SOUTH_EAST )
                            ballDirection = NORTH_EAST;
                        else
                            ballDirection = NORTH_WEST;
                    }
                }
            }
        }
    }
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

    if( isRectangleBoardArea )  {
        isRectangleBoardClick = true;

        leftTopX = x;
        leftTopY = y;
    }

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
        case RIGHT_BOTTOM:
            rightBottomX = x;
            rightBottomY = y;
        case LEFT_BOTTOM:
            leftBottomX = x;
            leftBottomY = y;
            emit signalBoardAreaReady(false);
            break;
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

    if( isRectangleBoardArea )  {
        isRectangleReady = true;
        isRectangleBoardArea = false;
        isRectangleBoardClick = false;

        rightBottomX = event->x();
        rightBottomY = event->y();

        emit signalBoardAreaPoint(LEFT_TOP, leftTopX, leftTopY);
        emit signalBoardAreaPoint(RIGHT_TOP, rightTopX, rightTopY);
        emit signalBoardAreaPoint(RIGHT_BOTTOM, rightBottomX, rightBottomY);
        emit signalBoardAreaPoint(LEFT_BOTTOM, leftBottomX, leftBottomY);

        emit signalBoardAreaReady(true);
    }
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

    if( isRectangleBoardClick ) {
        rightTopX = mousePosX;
        rightTopY = leftTopY;

        leftBottomX = leftTopX;
        leftBottomY = mousePosY;

        rightBottomX = mousePosX;
        rightBottomY = mousePosY;
    }
    else if( isMouseClicked )    {
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
    isRectangleBoardArea = false;

    this->hasFocus();

    boardAreaClick = LEFT_TOP;

    emit signalBoardAreaReady(false);
    emit signalBoardAreaPoint(RESET_BOARD_AREA, 0, 0);
}

/**
 * @brief ImageGLView::slotSetRectangleBoardArea
 */
void ImageGLView::slotSetRectangleBoardArea()
{
    leftTopX = leftTopY = 0;
    rightTopX = rightTopY = 0;
    leftBottomX = leftBottomY = 0;
    rightBottomY = rightBottomY = 0;

    isRectangleReady = false;
    isRectangleBoardArea = true;

    this->hasFocus();

    emit signalBoardAreaReady(false);
}

/**
 * @brief ImageGLView::slotBoardArea
 * @param _isBoardArea
 */
void ImageGLView::slotBoardArea(bool _isBoardArea)
{
    isBoardArea = _isBoardArea;
}

/**
 * @brief ImageGLView::slotRectangleReady
 * @param _isRectangleReady
 */
void ImageGLView::slotRectangleReady(bool _isRectangleReady)
{
    isRectangleReady = _isRectangleReady;
}

/**
 * @brief ImageGLView::slotFindBall
 * @param _ballPos
 */
void ImageGLView::slotFindBall(QPoint _ballPos)
{
    ballPos = _ballPos;

    if( ballPos.x() == 0 && ballPos.y() == 0 )  {
        ballFound = false;
    }
    else    {
        ballFound = true;
    }
}

/**
 * @brief ImageGLView::slotPredictGradient
 * @param _ballGradient
 */
void ImageGLView::slotPredictGradient(double _ballGradient)
{
    ballGradient = _ballGradient;
}

/**
 * @brief ImageGLView::slotBallMoving
 * @param _isBallMoving
 * @param _ballDirection
 */
void ImageGLView::slotBallMoving(bool _isBallMoving, BallDirection _ballDirection)
{
    isBallMoving = _isBallMoving;

    ballDirection = _ballDirection;
}

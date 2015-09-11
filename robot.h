#ifndef ROBOT_H
#define ROBOT_H

#include <QWidget>

class Robot : public QWidget
{
    Q_OBJECT

public:
    Robot();
    ~Robot();
    int unitX;
    int unitY;

    void updateColor(int, int, int);
    void resetColor();

    QColor getMaxColor();
    QColor getMinColor();
    void updatePosition(QPoint);

    QPoint getPosition();

private:
    QPoint position;

    QColor maxColor;
    QColor minColor;

    int redMax;
    int redMin;
    int greenMax;
    int greenMin;
    int blueMax;
    int blueMin;

public slots:
    void slotRenewPosition(QPoint);
};

#endif // ROBOT_H

#ifndef ROBOT_H
#define ROBOT_H

#include <QWidget>

class Robot : public QWidget
{
    Q_OBJECT

public:
    Robot();
    ~Robot();
    QColor maxColor;
    QColor minColor;
    int unitX;
    int unitY;

private:
    QPoint position;

public slots:
    void slotRenewPosition(QPoint);
};

#endif // ROBOT_H

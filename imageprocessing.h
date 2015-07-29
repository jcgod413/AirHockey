#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <QObject>
#include <QImage>
#include <QPixmap>
#include "ahr.h"

class ImageProcessing : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcessing(QObject *parent = 0);
    ~ImageProcessing();
    void erodeImage(QImage &sourceImage);

private:
    int dir[8][2] = { {-1,-1}, {0, -1}, {1, -1}, {1, 0},
                      {1, 1},  {0, 1}, {-1, 1}, {-1, 0}};

signals:

public slots:
};

#endif // IMAGEPROCESSING_H

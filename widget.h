#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QTimer>
#include "imageglview.h"
#include "imageprocessing.h"
#include "capturethread.h"
#include "ball.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    void initWidget();
    void initCamera();
    void initThread();

private:
    Ui::Widget *ui;

    QCamera *camera;
    QCameraViewfinder *viewFinder;
    QCameraImageCapture *imageCapture;
    QList<QByteArray> camDevNameLists;

    ImageGLView *imageGLView;
    ImageProcessing *imageProcessing;
    CaptureThread *captureThread;


public slots:
    void slotCameraStart();
    void slotCameraStop();
    void slotCameraCapture();

    void slotCamDisplayError();
    void slotCamImageCapture(int pId, QImage pPreview);
    void slotCaptureLoad();
    void slotUpdateCameraState(QCamera::State state);
    void slotThreadStart();
    void slotFindBall(QPoint);
};

#endif // WIDGET_H

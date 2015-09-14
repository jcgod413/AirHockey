#ifndef CAPTURETHREAD_H
#define CAPTURETHREAD_H

#include <QThread>
#include <QElapsedTimer>
#include "ahr.h"

class CaptureThread : public QThread
{
    Q_OBJECT
public:
    explicit CaptureThread();
    void run();
    void stop();

private:
    QElapsedTimer cameraTimer;
    bool isThreadRunning;

signals:
    void captureReady();

};

#endif // CAPTURETHREAD_H

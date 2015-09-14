#include "capturethread.h"

/**
 * @brief CaptureThread::CaptureThread
 */
CaptureThread::CaptureThread() :
    isThreadRunning(false)
{

}

/**
 * @brief CaptureThread::run
 */
void CaptureThread::run()
{
    cameraTimer.start();
    isThreadRunning = true;

    while(isThreadRunning)  {
        if( cameraTimer.elapsed() == TIMEOUT_INTERVAL ) {
            emit captureReady();
            cameraTimer.restart();
        }
    }
}

/**
 * @brief CaptureThread::stop
 */
void CaptureThread::stop()
{
    isThreadRunning = false;
}

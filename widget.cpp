#include "widget.h"
#include "ui_widget.h"

/**
 * @brief Widget::Widget
 * @param parent
 */
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget),
    imageProcessing(new ImageProcessing),
    camera(0),
    imageCapture(0)
{
    ui->setupUi(this);

    initWidget();
    initCamera();
    initThread();
}

/**
 * @brief Widget::~Widget
 */
Widget::~Widget()
{
    delete ui;
}

/**
 * @brief Widget::initWidget
 */
void Widget::initWidget()
{
    imageGLView = new ImageGLView();

    imageGLView->setMinimumSize(640, 480);
    ui->screenLayout->addWidget(imageGLView);

    ui->stopButton->setEnabled(false);
    ui->captureButton->setEnabled(false);
    ui->resetColorButton->setEnabled(false);
    ui->setBoardAreaButton->setEnabled(false);
    ui->setBoardRectangleButton->setEnabled(false);
    ui->morpologyEnableCheck->setEnabled(false);

    connect(ui->startButton, SIGNAL(clicked()),
                             SLOT(slotCameraStart()));
    connect(ui->stopButton, SIGNAL(clicked()),
                            SLOT(slotCameraStop()));
    connect(ui->captureButton, SIGNAL(clicked()),
                               SLOT(slotCameraCapture()));

    connect(ui->resetColorButton, SIGNAL(clicked()),
            imageProcessing, SLOT(slotResetMaskColor()));
    connect(ui->setBoardAreaButton, SIGNAL(clicked()),
            imageGLView, SLOT(slotSetBoardArea()));
    connect(ui->setBoardRectangleButton, SIGNAL(clicked()),
            imageGLView, SLOT(slotSetRectangleBoardArea()));
    connect(ui->morpologyEnableCheck, SIGNAL(toggled(bool)),
            imageProcessing, SLOT(slotMorpologyEnable(bool)));

    connect(imageGLView, SIGNAL(signalDraggedImage(int,int)),
            imageProcessing, SLOT(slotDraggedImage(int,int)));
    connect(imageGLView, SIGNAL(signalBoardAreaPoint(int,int,int)),
            imageProcessing, SLOT(slotBoardAreaPoint(int,int,int)));
    connect(imageGLView, SIGNAL(signalBoardAreaReady(bool)),
            imageProcessing, SLOT(slotBoardAreaReady(bool)));

    connect(imageProcessing, SIGNAL(signalBoardArea(bool)),
            imageGLView, SLOT(slotBoardArea(bool)));
    connect(imageProcessing, SIGNAL(signalRectangleReady(bool)),
            imageGLView, SLOT(slotRectangleReady(bool)));
    connect(imageProcessing, SIGNAL(signalBoardArea(bool)),
            imageGLView, SLOT(slotBoardArea(bool)));

    connect(imageProcessing, SIGNAL(signalFindBall(QPoint)),
            this, SLOT(slotFindBall(QPoint)));
}

/**
 * @brief Widget::initCamera
 */
void Widget::initCamera()
{
    viewFinder = new QCameraViewfinder();
    viewFinder->hide();

    camDevNameLists.clear();
    ui->comboBox->clear();

    foreach(const QByteArray &deviceName, QCamera::availableDevices())  {
        QString description = camera->deviceDescription(deviceName);
        camDevNameLists.append(deviceName);
        ui->comboBox->addItem(description);
    }
}

/**
 * @brief Widget::initThread
 */
void Widget::initThread()
{
    captureThread = new CaptureThread();

    connect(captureThread, SIGNAL(captureReady()),
                           SLOT(slotCaptureLoad()));
}

/**
 * @brief Widget::slotThreadStart
 */
void Widget::slotThreadStart()
{
   if( !captureThread->isRunning() )    {
       captureThread->start();
       ui->startButton->setEnabled(false);
       ui->stopButton->setEnabled(true);
       ui->captureButton->setEnabled(true);
       ui->resetColorButton->setEnabled(true);
       ui->setBoardAreaButton->setEnabled(true);
       ui->setBoardRectangleButton->setEnabled(true);
       ui->morpologyEnableCheck->setEnabled(false);
   }
}

/**
 * @brief Widget::slotCameraStart
 */
void Widget::slotCameraStart()
{
    delete imageCapture;
    delete camera;

    if( camDevNameLists.count() < 1 )
        camera = new QCamera();
    else    {
        int currentIndex = ui->comboBox->currentIndex();
        camera = new QCamera(camDevNameLists.at(currentIndex));
    }

    imageCapture = new QCameraImageCapture(camera);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    imageCapture->setBufferFormat(QVideoFrame::Format_RGB32);

    connect(imageCapture, SIGNAL(imageCaptured(int,QImage)),
                          SLOT(slotCamImageCapture(int,QImage)));
    connect(camera, SIGNAL(stateChanged(QCamera::State)),
                    SLOT(slotUpdateCameraState(QCamera::State)));
    connect(camera, SIGNAL(error(QCamera::Error)),
                    SLOT(slotCamDisplayError()));

    camera->setViewfinder(viewFinder);
    camera->setCaptureMode(QCamera::CaptureStillImage);

    camera->start();
}

/**
 * @brief Widget::slotCameraStop
 */
void Widget::slotCameraStop()
{
    camera->stop();
}

/**
 * @brief Widget::slotCamDisplayError
 */
void Widget::slotCamDisplayError()
{
    qDebug("slotCamDisplayError");
}

/**
 * @brief Widget::slotUpdateCameraState
 */
void Widget::slotUpdateCameraState(QCamera::State state)
{
    switch( state ) {
    case QCamera::ActiveState:
        qDebug("Activate...");
        QTimer::singleShot(3000, this, SLOT(slotThreadStart()));
        break;
    case QCamera::UnloadedState:
        break;
    case QCamera::LoadedState:
        ui->startButton->setEnabled(true);
        ui->stopButton->setEnabled(false);
        break;
    }
}

/**
 * @brief Widget::slotCameraCapture
 */
void Widget::slotCameraCapture()
{
    qDebug("capture..");

    imageCapture->capture("/Users/JaeCheol/Desktop/capture.jpg");
}

/**
 * @brief Widget::slotCamImageCapture
 * @param pId
 * @param pPreview
 */
void Widget::slotCamImageCapture(int pId, QImage pPreview)
{
    Q_UNUSED(pId);
    qDebug() << "IMAGE: " << pPreview.byteCount();
}

/**
 * @brief Widget::slotCaptureLoad
 */
void Widget::slotCaptureLoad()
{
    QRect rect(0, 0, viewFinder->width(), viewFinder->height());
    QPixmap captureImage = viewFinder->grab(rect).scaled(640, 480);
    QImage frameImage = captureImage.toImage();

    imageProcessing->loadRawImage(frameImage);

    imageGLView->imageLoad(imageProcessing->getThresholdImage());
}

/**
 * @brief Widget::slotFindBall
 * @param ballPosition
 */
void Widget::slotFindBall(QPoint ballPosition)
{
    ui->ballPositionX->setNum(ballPosition.x());
    ui->ballPositionY->setNum(ballPosition.y());
}

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
    tactics(new Tactics),
    camera(0),
    imageCapture(0),
    radioState(RADIO_BALL)
{
    ui->setupUi(this);

    initWidget();
    initCamera();
    initBluetoothPort();
    initThread();
    initTestSerial();
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

    imageGLView->setMinimumSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    ui->screenLayout->addWidget(imageGLView);

    ui->stopButton->setEnabled(false);
    ui->captureButton->setEnabled(false);
    ui->resetColorButton->setEnabled(false);
    ui->setBoardAreaButton->setEnabled(false);
    ui->setBoardRectangleButton->setEnabled(false);
    ui->morpologyEnableCheck->setEnabled(false);

    ui->radioBallButton->click();

    connect(ui->startButton, SIGNAL(clicked()),
                             SLOT(slotCameraStart()));
    connect(ui->stopButton, SIGNAL(clicked()),
                            SLOT(slotCameraStop()));
    connect(ui->captureButton, SIGNAL(clicked()),
                               SLOT(slotCameraCapture()));
    connect(ui->robotSideSlider, SIGNAL(valueChanged(int)),
            imageProcessing, SLOT(slotRobotSideChanged(int)));
    connect(ui->robotSideSlider, SIGNAL(valueChanged(int)),
            tactics, SLOT(slotRobotSideChanged(int)));

    connect(ui->resetColorButton, SIGNAL(clicked()),
            imageProcessing, SLOT(slotResetMaskColor()));
    connect(ui->morpologyEnableCheck, SIGNAL(toggled(bool)),
            imageProcessing, SLOT(slotMorpologyEnable(bool)));

    connect(ui->bluetoothComboBox, SIGNAL(currentIndexChanged(QString)),
            tactics, SLOT(slotPortNameChanged(QString)));

    connect(imageProcessing, SIGNAL(signalFindBall(QPoint)),
            this, SLOT(slotFindBall(QPoint)));
    connect(imageProcessing, SIGNAL(signalFindRobot(QPoint)),
            this, SLOT(slotFindRobot(QPoint)));

    connect(imageGLView, SIGNAL(signalMouseLeftClick(QPoint)),
            imageProcessing, SLOT(slotMouseClick(QPoint)));
    connect(imageGLView, SIGNAL(signalMouseMove(QPoint)),
            imageProcessing, SLOT(slotMouseMove(QPoint)));
    connect(imageGLView, SIGNAL(signalMouseRelease(QPoint)),
            imageProcessing, SLOT(slotMouseRelease(QPoint)));

    connect(ui->setBoardRectangleButton, SIGNAL(clicked()),
            imageProcessing, SLOT(slotSetRectangleBoardArea()));

    connect(imageProcessing, SIGNAL(signalRenewObjects(Ball*,Robot*)),
            tactics, SLOT(slotRenewObjects(Ball*,Robot*)));
    connect(imageProcessing, SIGNAL(signalImageProcessCompleted()),
            tactics, SLOT(slotStartAction()));

    connect(ui->radioBallButton, SIGNAL(clicked()),
            this, SLOT(slotRadioBall()));
    connect(ui->radioRobotButton, SIGNAL(clicked()),
            this, SLOT(slotRadioRobot()));
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

void Widget::initBluetoothPort()
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->bluetoothComboBox->addItem(info.portName());
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
 * @brief Widget::initTestMotion
 */
void Widget::initTestSerial()
{
    connect(ui->serialSendButton, SIGNAL(clicked()),
            this, SLOT(slotSendSerial()));
}

/**
 * @brief Widget::slotSendSerial
 */
void Widget::slotSendSerial()
{
    QByteArray packet;
    packet.append(ui->serialEditText->text());
//    packet.append("S")
//            .append("0")
//            .append(ui->serialEditText->text())
//            .append("E");

    tactics->sendSerial(packet);
}

/**
 * @brief Widget::slotRadioBall
 */
void Widget::slotRadioBall()
{
    imageProcessing->radioStateChanged(radioState = RADIO_BALL);
}

/**
 * @brief Widget::slotRadioRobot
 */
void Widget::slotRadioRobot()
{
    imageProcessing->radioStateChanged(radioState = RADIO_ROBOT);
}

/**
 * @brief Widget::slotThreadStart
 */
void Widget::slotThreadStart()
{
   if( !captureThread->isRunning() )    {
       captureThread->start();
       ui->stopButton->setEnabled(true);
       ui->startButton->setEnabled(false);
       ui->captureButton->setEnabled(true);
       ui->resetColorButton->setEnabled(true);
       ui->setBoardAreaButton->setEnabled(true);
       ui->morpologyEnableCheck->setEnabled(true);
       ui->setBoardRectangleButton->setEnabled(true);
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
    QPixmap captureImage = viewFinder->grab(rect).scaled(SCREEN_WIDTH, SCREEN_HEIGHT);
    QImage frameImage = captureImage.toImage();
    QImage resultImage = imageProcessing->imageProcess(&frameImage);

    imageGLView->imageLoad(resultImage);
}

/**
 * @brief Widget::slotFindBall
 * @param ballPosition
 */
void Widget::slotFindBall(QPoint ballPosition)
{
    QString ballPosText;

    if( ballPosition.x() == 0 && ballPosition.y() == 0 )  {
        ballPosText = "Not found";
    }
    else    {
        ballPosText.append("x:");
        ballPosText.append(ballPosText.number(ballPosition.x()));
        ballPosText.append(" ");
        ballPosText.append("y:");
        ballPosText.append(ballPosText.number(ballPosition.y()));
    }

    ui->ballPosLabel->setText(ballPosText);
}

/**
 * @brief Widget::slotFindRobot
 * @param robotPosition
 */
void Widget::slotFindRobot(QPoint robotPosition)
{
    QString robotPosText;

    if( robotPosition.x() == 0 && robotPosition.y() == 0 )  {
        robotPosText = "Not found";
    }
    else    {
        robotPosText.append("x:");
        robotPosText.append(robotPosText.number(robotPosition.x()));
        robotPosText.append(" ");
        robotPosText.append("y:");
        robotPosText.append(robotPosText.number(robotPosition.y()));
    }

    ui->robotPosLabel->setText(robotPosText);
}

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    frameGrabber = new FrameGrabber();
    initialSetup();
}

MainWindow::~MainWindow()
{
    delete frameGrabber;
    delete ui;
}

void MainWindow::initialSetup()
{
    ui->labelShowFrame->setStyleSheet("background-color: rgb(253, 253, 253);");
    // N means No Grabbing
    grabMode = 'N';
    // Set button initial status and color
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonCapture->setEnabled(false);
    ui->pushButtonStream->setEnabled(false);
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonSaveCapture->setEnabled(false);
    ui->pushButtonScanDevices->setEnabled(true);
    ui->pushButtonStop->setStyleSheet("background-color: rgb(225, 225, 225);");
    ui->pushButtonConnect->setStyleSheet("background-color: rgb(225, 225, 225);");
    ui->pushButtonDisconnect->setStyleSheet("background-color: rgb(225, 225, 225);");
    ui->pushButtonCapture->setStyleSheet("background-color: rgb(225, 225, 225);");
    ui->pushButtonStream->setStyleSheet("background-color: rgb(225, 225, 225);");
    ui->pushButtonScanDevices->setStyleSheet("background-color: rgb(225, 225, 225);");
    ui->pushButtonSaveCapture->setStyleSheet("background-color: rgb(225, 225, 225);");
    // Interation between UI and frameGrabber
    connect(this, SIGNAL(sendConnect()), frameGrabber, SLOT(receiveConnectCamera()));
    connect(this, SIGNAL(sendDisconnect()), frameGrabber, SLOT(receiveDisconnectCamera()));
    connect(this, SIGNAL(sendCaptureMode()), frameGrabber, SLOT(receiveStartCaptureMode()));
    connect(this, SIGNAL(sendStreamMode()), frameGrabber, SLOT(receiveStartStreamMode()));
    connect(this, SIGNAL(sendStopGrabbing()), frameGrabber, SLOT(receiveStopGrabbing()));
    connect(frameGrabber, SIGNAL(sendCaptureFrame(cv::Mat)), this, SLOT(receiveRawFrame(cv::Mat)));

    ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    System started.");
}

void MainWindow::on_pushButtonConnect_clicked()
{
    emit sendConnect();
    usleep(5000); // 5ms

    if (frameGrabber->cameraConnected) {
        ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Camera is open.");

        ui->pushButtonConnect->setEnabled(false);
        ui->pushButtonConnect->setStyleSheet("background-color: rgb(100, 255, 100);");
        ui->pushButtonDisconnect->setEnabled(true);
        ui->pushButtonCapture->setEnabled(true);
        ui->pushButtonStream->setEnabled(true);
    }
    else {
        ui->listWidgetMessageLog->addItem("[Error]   " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Fail to connect camera.");
        QMessageBox::information(this, "Connetion Failure", "Please Check Camera and Debug.");
    }
}

void MainWindow::on_pushButtonDisconnect_clicked()
{
    if (grabMode == 'N') {
        emit sendDisconnect();
        usleep(5000);
        if (!frameGrabber->cameraConnected) {
            ui->pushButtonDisconnect->setEnabled(false);
            ui->pushButtonConnect->setEnabled(true);
            ui->pushButtonConnect->setStyleSheet("background-color: rgb(225, 225, 225);");
            ui->pushButtonCapture->setEnabled(false);
            ui->pushButtonStream->setEnabled(false);
            ui->pushButtonSaveCapture->setEnabled(false);
            ui->pushButtonStop->setEnabled(false);
            ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Camera is close.");
        }
        else {
            ui->listWidgetMessageLog->addItem("[Error]   " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Fail to disconnect camera.");
            QMessageBox::information(this, "Disconnetion Failure", "Please Debug.");
        }
    }
    else if (grabMode == 'S') {
        QMessageBox::StandardButton disconnectReply;
        disconnectReply = QMessageBox::question(this, "Disconnect",
                                                "Camera is still in streaming mode. Are you sure to disconnect camera?",
                                                QMessageBox::Yes|QMessageBox::No);
        if (disconnectReply == QMessageBox::Yes) {
            emit sendDisconnect();
            usleep(5000);
            if (!frameGrabber->cameraConnected) {
                on_pushButtonStop_clicked();
                ui->pushButtonCapture->setEnabled(false);
                ui->pushButtonStream->setEnabled(false);
                ui->pushButtonStream->setStyleSheet("background-color: rgb(225, 225, 225);");
                ui->pushButtonDisconnect->setEnabled(false);
                ui->pushButtonSaveCapture->setEnabled(false);
                ui->pushButtonConnect->setEnabled(true);
                ui->pushButtonConnect->setStyleSheet("background-color: rgb(225, 225, 225);");
                ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Camera is close.");
            }
          }
    }
    else if (grabMode == 'C') {
        emit sendDisconnect();
        usleep(5000);
        if (!frameGrabber->cameraConnected) {
            ui->pushButtonCapture->setEnabled(false);
            ui->pushButtonStream->setEnabled(false);
            ui->pushButtonStream->setStyleSheet("background-color: rgb(225, 225, 225);");
            ui->pushButtonDisconnect->setEnabled(false);
            ui->pushButtonSaveCapture->setEnabled(false);
            ui->pushButtonConnect->setEnabled(true);
            ui->pushButtonConnect->setStyleSheet("background-color: rgb(225, 225, 225);");
            ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Camera is close.");
        }
    }
}

void MainWindow::on_pushButtonCapture_clicked()
{
    emit sendCaptureMode();

    ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Capture one image.");
    grabMode = 'C';
    ui->pushButtonStream->setEnabled(true);
    ui->pushButtonStop->setEnabled(false);
    ui->pushButtonSaveCapture->setEnabled(true);
}

void MainWindow::on_pushButtonSaveCapture_clicked()
{
    QString filePath = "../images/" + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss") + ".jpg";
    QByteArray ba = filePath.toLatin1();
    const char *fileName = ba.data();

    if (cv::imwrite(fileName, cvRawFrameCopy)){
        ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Image saved.");
    }
}

void MainWindow::on_pushButtonStream_clicked()
{  
    grabMode = 'S';
    ui->pushButtonStream->setEnabled(false);
    ui->pushButtonStream->setStyleSheet("background-color: rgb(100, 255, 100);");
    ui->pushButtonCapture->setEnabled(false);
    ui->pushButtonStop->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(255, 0, 0);");
    ui->pushButtonStop->setEnabled(true);
    ui->pushButtonSaveCapture->setEnabled(false);

    emit sendStreamMode();

    streamTrigger = new QTimer();
    streamTrigger->setInterval(1);

    connect(streamTrigger, SIGNAL(timeout()), frameGrabber, SLOT(receiveSendFrame()));
    connect(frameGrabber, SIGNAL(sendFrame(cv::Mat)), this, SLOT(receiveRawFrame(cv::Mat)));

    streamTrigger->start();

    ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Start streaming mode.");
}

void MainWindow::on_pushButtonStop_clicked()
{
    if ( grabMode == 'S' ) {
        delete streamTrigger;
        emit sendStopGrabbing();
        usleep(5000);

        if (!frameGrabber->startGrabbing) {
            grabMode = 'N';
            ui->pushButtonCapture->setEnabled(true);
            ui->pushButtonStream->setEnabled(true);
            ui->pushButtonStream->setStyleSheet("background-color: rgb(225, 225, 225);");
            ui->pushButtonStop->setStyleSheet("background-color: rgb(225 225, 225);");
            ui->pushButtonStop->setEnabled(false);
            ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Quit streaming mode.");
        }
    }
}

void MainWindow::on_pushButtonScanDevices_clicked()
{
    ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Scanning devices...");
    QString deviceName = frameGrabber->scanDevices();

    if (deviceName.contains("Basler", Qt::CaseSensitive)) {
        ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    Camera found.");
        if (!frameGrabber->cameraConnected) {
            ui->pushButtonConnect->setEnabled(true);
        }
    }
    else {
        ui->listWidgetMessageLog->addItem("[Info]    " + QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") + "    No camera found, please check connection.");
        ui->pushButtonConnect->setEnabled(false);
    }

    ui->comboBoxDevices->clear();
    ui->comboBoxDevices->addItem(deviceName);
}

void MainWindow::receiveRawFrame(cv::Mat cvRawFrame)
{
    cvRawFrameCopy = cvRawFrame.clone();
    displayFrame(cvRawFrame);
}

void MainWindow::displayFrame(cv::Mat cvDisplayFrame)
{
    cv::cvtColor(cvDisplayFrame, cvDisplayFrame, cv::COLOR_BGR2RGB);
    cv::resize(cvDisplayFrame, cvResizedFrame, cv::Size(1024, 768));
    qDisplayedFrame = QImage((uchar*)cvResizedFrame.data, cvResizedFrame.cols, cvResizedFrame.rows, cvResizedFrame.step, QImage::Format_RGB888);
    ui->labelShowFrame->setPixmap(QPixmap::fromImage(qDisplayedFrame));
}
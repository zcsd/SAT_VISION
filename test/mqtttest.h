// MqttTest class: test MQTT connection and function
// Author: @ZC
// Date: created on 10 Oct 2018
#ifndef MQTTTEST_H
#define MQTTTEST_H

#include <QWidget>
#include <QObject>
#include <QDebug>
#include <QDateTime>

#include "communication/mqttclient.h"

namespace Ui {
class MqttTest;
}

class MqttTest : public QWidget
{
    Q_OBJECT

public:
    explicit MqttTest(QWidget *parent = nullptr);
    ~MqttTest();

private slots:
    void on_pushButtonConnect_clicked();
    void on_pushButtonDisconnect_clicked();
    void on_pushButtonPublish_clicked();
    void on_pushButtonSub_clicked();
    void on_pushButtonUnsub_clicked();

    void receiveConState(int state);
    void receiveSubState(int state);
    void receiveSubMsg(QString topic, QString msg);

private:
    Ui::MqttTest *ui;
    MqttClient *mqttClient;
    bool isConnected = false, isSub = false;
    void initSetup();
};

#endif // MQTTTEST_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QThread>

#include "customlabel.h"//============

#include "autopilot.h"
#include "gps.h"
#include "bluetoothserver.h"
#include "sensorreader.h"
#include "slavecontroller.h"
#include "drawtrack.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateLatLon(QGeoCoordinate &coord);

    void updateDataSens(QVector3D accel, QVector3D gyro);

    void readFromSlave14(int &data);

    void connectedClient(const QString &name);
    void disconnectedClient(const QString &name);

    //void buttonFiltrKalmana();

private:


    Ui::MainWindow *ui;


    Autopilot* autopilot;
    QThread* threadAutopilot;
    QTimer* timerAutopilot;

    GPS* gps;
    QThread* threadGPS;

    BluetoothServer* bluetoothserver;

    SensorReader* sensorreader;
    QThread* threadSensorReader;
    QTimer* timerSensorReader;

    SlaveController* slavecontroller_14;
    QThread* threadSlaveController_14;

    DrawTrack *drawtrack;
    QThread* threadDrawTrack;
    QTimer* timerDrawTrack;
};
#endif // MAINWINDOW_H

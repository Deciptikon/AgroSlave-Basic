#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QRect sizeWindow = MainWindow::geometry();
    ui->lb_draw_track->setWindowGeometry(sizeWindow);




    ///-------Create autopilot and move to thread with timer----------------------------------------
    autopilot = new Autopilot(this);
    autopilot->init(1000);

    threadAutopilot = new QThread(this);

    timerAutopilot = new QTimer(0);
    timerAutopilot->setInterval(autopilot->getMSecDeltaTime());
    timerAutopilot->moveToThread(threadAutopilot);

    autopilot->connect( timerAutopilot, SIGNAL(timeout()), SLOT(loop()), Qt::ConnectionType::DirectConnection);
    timerAutopilot->connect(threadAutopilot, SIGNAL(started()), SLOT(start()));
    ///----------------------------------------------------------------------------------------------



    ///-------Create GPS-reader----------------------------------------------------------------------
    gps = new GPS(this);
    gps->init();
    gps->setMsecUpdate(1000);

    threadGPS = new QThread(this);

    gps->moveToThread(threadGPS);

    //connect(ui->bt_filtr_GPS, SIGNAL(clicked()), gps, SLOT(onOffFiltrKalmana()));
    connect(gps, SIGNAL(updatePositionGeo(QGeoCoordinate&)), this, SLOT(updateLatLon(QGeoCoordinate&)));

    ///----------------------------------------------------------------------------------------------



    ///-------Create Bluetooth server----------------------------------------------------------------
    bluetoothserver = new BluetoothServer(this);
    connect(bluetoothserver, SIGNAL(clientConnected(QString)), this, SLOT(connectedClient(QString)));
    connect(bluetoothserver, SIGNAL(clientDisconnected(QString)), this, SLOT(disconnectedClient(QString)));
    bluetoothserver->startServer();
    ///----------------------------------------------------------------------------------------------



    ///-------Create sensor reader (I2C)-------------------------------------------------------------
    sensorreader = new SensorReader(this);
    sensorreader->init(20);

    threadSensorReader = new QThread(this);

    timerSensorReader = new QTimer(0);
    timerSensorReader->setInterval(sensorreader->getMsecDeltaTime());
    timerSensorReader->moveToThread(threadSensorReader);

    sensorreader->connect(timerSensorReader,  SIGNAL(timeout()), SLOT(loop()), Qt::ConnectionType::DirectConnection);
    timerSensorReader->connect(threadSensorReader, SIGNAL(started()), SLOT(start()));
    connect(sensorreader, SIGNAL(updateDataSens(QVector3D, QVector3D)),
            this, SLOT(updateDataSens(QVector3D, QVector3D)) );
//    connect(sensorreader, SIGNAL(updateDataSens(QVector3D, QVector3D)),
//            autopilot, SLOT(updateDataSens(QVector3D, QVector3D)) );
    connect(ui->bt_calibrate_sensors, SIGNAL(clicked()),
            sensorreader, SLOT(calibrate()) );
    connect(ui->bt_calibrate_sensors, SIGNAL(clicked()),
                    autopilot, SLOT(resetDataSens()) );
    ///----------------------------------------------------------------------------------------------


    ///-------Create I2C slave device----------------------------------------------------------------
    slavecontroller_14 = new SlaveController(this);
    slavecontroller_14->init( 0x14 );

    threadSlaveController_14 = new QThread(this);
    slavecontroller_14->moveToThread(threadSlaveController_14);

    connect(ui->bt_send_read_command, SIGNAL(clicked()), slavecontroller_14, SLOT(writeData()));
    connect(autopilot, SIGNAL(sendCommandToSlave14(int&)), slavecontroller_14, SLOT(writeData(int&)));
    connect(slavecontroller_14, SIGNAL(readFrom(int&)), this, SLOT(readFromSlave14(int&)));

    ///----------------------------------------------------------------------------------------------

    ///-------Create DrawTrack-----------------------------------------------------------------------
    drawtrack = new DrawTrack(this);
    drawtrack->init(33, 300, 300);

    threadDrawTrack = new QThread(this);

    timerDrawTrack = new QTimer(0);
    timerDrawTrack->setInterval(drawtrack->getMsecDeltaTime());
    timerDrawTrack->moveToThread(threadDrawTrack);

    drawtrack->connect(timerDrawTrack,  SIGNAL(timeout()), SLOT(loop()), Qt::ConnectionType::DirectConnection);
    timerDrawTrack->connect(threadDrawTrack, SIGNAL(started()), SLOT(start()));

    connect(gps, SIGNAL(updatePositionXY(const double&,const double&)),
            drawtrack, SLOT(updateGPS(const double&,const double&)) );
    connect(drawtrack, SIGNAL(drawImage(const QImage&)), ui->lb_draw_track, SLOT(setImage(const QImage&)));
    ///----------------------------------------------------------------------------------------------

    ///-------Connects objects-----------------------------------------------------------------------
    connect(gps, SIGNAL(updatePositionLatLon(double&,double&)), bluetoothserver, SLOT(  sendCoord(double&,double&)) );
    connect(gps, SIGNAL(updatePositionXY(const double&,const double&)),
            autopilot, SLOT(readFromGPS(const double&,const double&)) );
    connect(bluetoothserver, SIGNAL(messageReceived(QByteArray)), autopilot, SLOT(readFromBluetoothServer(QByteArray)) );
    connect(sensorreader,  SIGNAL(updateDataSens(const QVector3D&, const QVector3D& )),
            autopilot, SLOT(readFromSensorReader(const QVector3D&, const QVector3D& )) );

    connect(autopilot, SIGNAL(sendCommandToSlave14(int&)),
            slavecontroller_14, SLOT(writeData(int&)) );
    connect(autopilot, SIGNAL(sendToAndroid(QByteArray)),
            bluetoothserver, SLOT(sendMessage(QByteArray)) );
    ///----------------------------------------------------------------------------------------------



    ///-------Start threads--------------------------------------------------------------------------
    threadAutopilot->start();
    threadGPS->start();
    threadSensorReader->start();
    threadSlaveController_14->start();
    threadDrawTrack->start();
    ///----------------------------------------------------------------------------------------------
}

MainWindow::~MainWindow()
{
    delete bluetoothserver;
    delete ui;
}

void MainWindow::updateLatLon(QGeoCoordinate &coord)
{
    if(coord.isValid()) {
        ui->lb_latitude->setText(QString::number(coord.latitude(), 'g', 9));
        ui->lb_longitude->setText(QString::number(coord.longitude(), 'g', 9));
    } else {
        ui->lb_latitude->setText("Invalid");
        ui->lb_longitude->setText("Invalid");
    }
}

void MainWindow::updateDataSens(QVector3D accel, QVector3D gyro)
{
//    qDebug() << "updateSensors Accelerometer:   " << accel;
//    qDebug() << "updateSensors Gyroscope:       " << gyro ;

    ui->lb_x_accelerometer->setText(QString::number(accel.x()));
    ui->lb_y_accelerometer->setText(QString::number(accel.y()));
    ui->lb_z_accelerometer->setText(QString::number(accel.z()));

    ui->lb_x_gyroscope->setText(QString::number(gyro.x()));
    ui->lb_y_gyroscope->setText(QString::number(gyro.y()));
    ui->lb_z_gyroscope->setText(QString::number(gyro.z()));
}

void MainWindow::readFromSlave14(int &data)
{
    ui->lb_slave_14->setText(QString::number(data));
}

void MainWindow::connectedClient(const QString &name)
{
    qDebug() << "MainWindow::connectedClient()";
    if(!name.isNull() && !name.isEmpty()) {
        ui->lb_btserver_on_off->setText("connected " + name);
    } else {
        ui->lb_btserver_on_off->setText("connected");
    }
}

void MainWindow::disconnectedClient(const QString &name)
{
    qDebug() << "MainWindow::disconnectedClient()";
    if(!name.isNull() && !name.isEmpty()) {
        ui->lb_btserver_on_off->setText("disconnected " + name);
    } else {
        ui->lb_btserver_on_off->setText("disconnected");
    }
}

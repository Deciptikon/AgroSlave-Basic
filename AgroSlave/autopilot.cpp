#include "autopilot.h"

Autopilot::Autopilot(QObject *parent) : QObject(parent)
{

}

void Autopilot::init(const int msecDeltaTime)
{
    if(msecDeltaTime > 1) {
        this->msecDeltaTime = msecDeltaTime;
    } else {
        this->msecDeltaTime = 1;
    }
    velocity  = QVector3D(0,0,0);
    translate = QVector3D(0,0,0);
    angle     = QVector3D(0,0,0);
}

void Autopilot::loop()
{
    qDebug() << "void Autopilot::loop()-------------------------------------------------";

    //update velocity and translate
    velocity += accelerometer*(float)msecDeltaTime/1000.0;
    translate+= velocity     *(float)msecDeltaTime/1000.0;
    qDebug() << "void Autopilot::accelerometer =" << accelerometer;
    qDebug() << "void Autopilot::velocity      =" << velocity;
    qDebug() << "void Autopilot::translate     =" << translate;
    accelerometer = QVector3D(0,0,0);


    angle    += gyroscope    *(float)msecDeltaTime/1000.0;
    qDebug() << "void Autopilot::gyroscope =" << gyroscope;
    qDebug() << "void Autopilot::angle     =" << angle;
    gyroscope     = QVector3D(0,0,0);

}

void Autopilot::readFromSensorReader(const QVector3D &accel, const QVector3D &gyro)
{
    //qDebug() << "Autopilot::readFromSensorReader(const QVector3D &accel, const QVector3D &gyro)";
    //QVector3D a={0,0,0};

    //qDebug() << "updateSensors Accelerometer:   " << accel;
    //qDebug() << "updateSensors Gyroscope:       " << gyro ;
    if(!accel.isNull()) {
        accelerometer += accel;
        //velocity += accelerometer*(float)msecDeltaTime/1000.0;
    }
    if(!gyro.isNull()) {
        gyroscope += gyro;
    }
}

void Autopilot::readFromBluetoothServer(const QByteArray &message)
{
    qDebug() << "Autopilot::readFromBluetoothServer(const QByteArray &message)";

    if(message.size()==1) {
            //qDebug() << "TEST";
            //QByteArray b;
            char p=message.at(0);
            switch (p) {
            case '4':

                break;
            case '5':
                //
                break;
            case '6':
                //
                break;
            case '7'://меняем состояние реле
                restateRelay();
                break;
            case '8':
                //
                break;


            default:
                qDebug() << "default: " << (int)p;
                break;
            }
        } else {//проверяем команду с параметрами
//            char p=message.at(0);
//            switch (p) {
//            case 0x01:
//                processKeyPoint(message);
//                break;

//            case 0x03:
//                b.append(message);
//                b.append('\n');
//                qDebug() << "Rotate: " << b.toHex();
//                sendToArduino(b);
//                break;


//            default:
//                qDebug() << "default: " << (int)p;
//                break;
//            }
        }
}

void Autopilot::readFromGPS(const double &x, const double &y)
{
    qDebug() << "Autopilot::readFromGPS(double &x, double &y)";
}

void Autopilot::resetDataSens()
{
    accelerometer = QVector3D(0,0,0);
    velocity      = QVector3D(0,0,0);
    translate     = QVector3D(0,0,0);
    gyroscope     = QVector3D(0,0,0);
    angle         = QVector3D(0,0,0);
}

int Autopilot::getMSecDeltaTime() const
{
    return msecDeltaTime;
}

void Autopilot::restateRelay()
{
    stateRelay = !stateRelay;
    qDebug() << "Состояние реле изменено на " << stateRelay;

    int c='c';
    emit sendCommandToSlave14(c);
}

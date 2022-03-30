#include "sensorreader.h"

SensorReader::SensorReader(QObject *parent) : QObject(parent)
{
    //
}

void SensorReader::init(const int msec)
{
    if(msec > 1) {
        msecDeltaTime = msec;
    } else {
        msecDeltaTime = 1;
    }
    isCalibrate=false;

    Accelerometer.init();
    Gyroscope.init();
}

void SensorReader::loop()
{
    Accelerometer.updateData();
    Gyroscope.updateData();

    //-----------Calibrate sensors-------------------------------------------------------------
    if(isCalibrate) {
        if (countTimerCalibrate < MAX_COUNT_TIMER_CALIBRATE) {
            countTimerCalibrate++;

            QVector3D calibrateDataAccelerometer = (Accelerometer.getCalibrateData() * (countTimerCalibrate - 1) +
                                       Accelerometer.getData())/(float)countTimerCalibrate;
            Accelerometer.setCalibrateData(calibrateDataAccelerometer);
            qDebug() << "Accelerometer::calibrate() " << calibrateDataAccelerometer;

            QVector3D calibrateDataGyroscope = (Gyroscope.getCalibrateData() * (countTimerCalibrate - 1) +
                                       Gyroscope.getData())/(float)countTimerCalibrate;
            Gyroscope.setCalibrateData(calibrateDataGyroscope);
            qDebug() << "Gyroscope::calibrate() " << calibrateDataGyroscope;
        } else {
            isCalibrate=false;
        }
    }
    //------------------------------------------------------------------------------------------

//qDebug() << "SensorReader::loop()";
    QVector3D accelData = Accelerometer.getData() - Accelerometer.getCalibrateData();
    QVector3D gyrosData = Gyroscope.getData() - Gyroscope.getCalibrateData();

//    xa=accelData.x();
//    ya=accelData.y();
//    za=accelData.z();
//    xg=gyrosData.x();
//    yg=gyrosData.y();
//    zg=gyrosData.z();

    emit updateDataSens( accelData, gyrosData);
//    emit updateDataSens2( xa, ya, za, zg, yg, zg);

}

void SensorReader::calibrate()
{
    qDebug() << "==========================================CALIBRATE==============================================";
    isCalibrate=true;
    countTimerCalibrate=0;
    Accelerometer.setCalibrateData({0,0,0});
    Gyroscope.setCalibrateData({0,0,0});
}

int SensorReader::getMsecDeltaTime() const
{
    return msecDeltaTime;
}

#ifndef AUTOPILOT_H
#define AUTOPILOT_H

#include <QObject>
#include <QDebug>
#include <QtMath>
#include <QVector3D>

class Autopilot : public QObject
{
    Q_OBJECT
public:
    explicit Autopilot(QObject *parent = nullptr);

    void init(const int secDeltaTime);

    int getMSecDeltaTime() const;


    void restateRelay();

signals:
    void sendToAndroid(const QByteArray &mess);
    void sendCommandToSlave14(int &comm);

public slots:
    void loop();

    void readFromSensorReader(const QVector3D &accel, const QVector3D &gyro);
//    void readFromSensorReader2(const float &xa, const float &ya, const float &za,
//                               const float &xg, const float &yg, const float &zg);
    void readFromBluetoothServer(const QByteArray &mess);
    void readFromGPS(const double &x, const double &y);

    void resetDataSens();

private slots:

private:
    int msecDeltaTime;

    QVector3D accelerometer, gyroscope;

    QVector3D velocity, translate, angle;

    bool stateRelay; //состояние реле подруливающего устройства
};

#endif // AUTOPILOT_H

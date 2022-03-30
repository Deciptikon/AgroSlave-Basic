#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <QObject>
#include <QDebug>
#include <QVector3D>

#include <wiringPi.h>
#include <wiringPiI2C.h>

class Gyroscope : public QObject
{
    Q_OBJECT
public:
    explicit Gyroscope(QObject *parent = nullptr);

    void init();

    void updateData();

    QVector3D getData() const;

    QVector3D getCalibrateData() const;

    void setCalibrateData(const QVector3D &value);

signals:

private:
    QVector3D data;

    int deviceGyro;

    QVector3D calibrateData;

};

#endif // GYROSCOPE_H

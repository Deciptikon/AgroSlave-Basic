#include "gyroscope.h"

Gyroscope::Gyroscope(QObject *parent) : QObject(parent)
{
    // Gyroscope ITG3200--------------------------
    #define GYRO 0x68 //  when AD0 is connected to GND ,gyro address is 0x68.
    //#define GYRO 0x69   when AD0 is connected to VCC ,gyro address is 0x69
    #define GYRO_POWER_CTL 0x1B
    #define GYRO_XOUT_H    0x1D
    #define GYRO_XOUT_L    0x1E
    #define GYRO_YOUT_H    0x1F
    #define GYRO_YOUT_L    0x20
    #define GYRO_ZOUT_H    0x21
    #define GYRO_ZOUT_L    0x22

    #define G_SMPLRT_DIV 0x15
    #define G_DLPF_FS 0x16
    #define G_INT_CFG 0x17
    #define G_PWR_MGM 0x3E
    #define G_TO_READ 8 // 2 bytes for each axis x, y, z
    //------------------------------------------------
    deviceGyro = -1;
}

void Gyroscope::init()
{

        deviceGyro = wiringPiI2CSetup(GYRO);
        qDebug() << "Setup deviceGyroRegAdress =" << deviceGyro;

        if (deviceGyro == -1) {
            qDebug() << "[Gyroscope::init()] deviceGyro == -1";
        } else {
            qDebug() << "I2C communication successfully setup.\n";

            wiringPiI2CWriteReg8(GYRO, G_PWR_MGM, 0x00);
            wiringPiI2CWriteReg8(GYRO, G_SMPLRT_DIV, 0x07);
            wiringPiI2CWriteReg8(GYRO, G_DLPF_FS, 0x1E);
            wiringPiI2CWriteReg8(GYRO, G_INT_CFG, 0x00);
        }
}

void Gyroscope::updateData()
{
    if (deviceGyro == -1) {
        qDebug() << "[Gyroscope::loop()] deviceGyro == -1";
    } else {
        int dataXg = wiringPiI2CReadReg16(deviceGyro, GYRO_XOUT_L);
        int dataYg = wiringPiI2CReadReg16(deviceGyro, GYRO_YOUT_L);
        int dataZg = wiringPiI2CReadReg16(deviceGyro, GYRO_ZOUT_L);

        data = QVector3D(
                    -(~(int16_t)dataXg + 1) / 14.375,
                    -(~(int16_t)dataYg + 1) / 14.375,
                    -(~(int16_t)dataZg + 1) / 14.375);
        //qDebug() << "Gyroscope:" << data;
    }
}

QVector3D Gyroscope::getData() const
{
    return data;
}

QVector3D Gyroscope::getCalibrateData() const
{
    return calibrateData;
}

void Gyroscope::setCalibrateData(const QVector3D &value)
{
    calibrateData = value;
}

#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

class Mpu {
  public:
    Mpu();
    void init();

  private:
    Adafruit_MPU6050* _mpu = nullptr;
};

extern Mpu* mpu;

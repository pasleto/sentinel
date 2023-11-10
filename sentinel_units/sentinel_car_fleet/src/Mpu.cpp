#include "Mpu.h"
#include "Logger.h"
#include "config.h"
#include "pins.h"

Mpu* mpu = new Mpu();

Mpu::Mpu() {
  _mpu = new Adafruit_MPU6050();
  pinMode(ESP_MPU_INT, INPUT); // TODO - external pull resistor
}

void Mpu::init() {
  // attachInterrupt(ESP_MPU_INT, handle_mpu_interrupt_rising, RISING);
  // attachInterrupt(ESP_MPU_INT, handle_mpu_interrupt_falling, FALLING);
  // attachInterrupt(ESP_MPU_INT, handle_mpu_interrupt_change, CHANGE);
  if (!_mpu->begin(MPU6050_I2C_ADDRESS)) {
    Log->println(F("[MPU] Error"));
    while (1);
  }
  _mpu->setInterruptPinPolarity(true); // interupt is active LOW
  _mpu->setInterruptPinLatch(false); // interupt is automaticly cleared after 50 microseconds

  // The acceleration is measured in meters per second square (m/s2)
  // The angular velocity is measured in radians per seconds (rad/s)
  // The temperature is measured in Celsius degrees

  // _mpu->setAccelerometerRange(MPU6050_RANGE_2_G);
  // _mpu->setAccelerometerRange(MPU6050_RANGE_4_G);
  // _mpu->setAccelerometerRange(MPU6050_RANGE_8_G);
  // _mpu->setAccelerometerRange(MPU6050_RANGE_16_G);
  // _mpu->setGyroRange(MPU6050_RANGE_250_DEG);
  // _mpu->setGyroRange(MPU6050_RANGE_500_DEG);
  // _mpu->setGyroRange(MPU6050_RANGE_1000_DEG);
  // _mpu->setGyroRange(MPU6050_RANGE_2000_DEG);
  // _mpu->setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  // _mpu->setHighPassFilter(MPU6050_HIGHPASS_1_25_HZ);
  // _mpu->setHighPassFilter(MPU6050_HIGHPASS_2_5_HZ);
  // _mpu->setHighPassFilter(MPU6050_HIGHPASS_5_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_260_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_184_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_94_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_44_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_21_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_10_HZ);
  // _mpu->setFilterBandwidth(MPU6050_BAND_5_HZ);
  // _mpu->setMotionDetectionThreshold(1);
  // _mpu->setMotionDetectionDuration(20);
  // _mpu->setMotionInterrupt(true); // motion detection will trigger interrupt
  // _mpu->setTemperatureStandby(true); // disasble temperature sensor
}

#ifndef __MPU9250_H
#define __MPU9250_H

#define MPU6050_ADDR 0xD0
#define PWR_MGMT_1 0x6B
#define TheMemAdd 0x3B
#define MEDIAN_WINDOW 5

#include "stm32f4xx_hal.h" // Kullandığın işlemciye göre değişebilir

// Fonksiyon prototipini buraya ekle:
int data_parsing(uint8_t *data, float *accel_data, float *gyro_data);

#endif


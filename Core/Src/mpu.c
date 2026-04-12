#include "main.h"
#include "mpu9250.h"

int data_parsing(uint8_t *data, float *accel_data, float *gyro_data)
{

    accel_data[0] = (int16_t)(data[0] << 8 | data[1]) / 16384.0; // x
    accel_data[1] = (int16_t)(data[2] << 8 | data[3]) / 16384.0; // y
    accel_data[2] = (int16_t)(data[4] << 8 | data[5]) / 16384.0; // z
    
    gyro_data[0] = (int16_t)(data[8] << 8 | data[9]) / 131.0; // X-
    gyro_data[1] = (int16_t)(data[10] << 8 | data[11]) / 131.0; // Y-
    gyro_data[2] = (int16_t)(data[12] << 8 | data[13]) / 131.0; // Z-

    return 0; 
}
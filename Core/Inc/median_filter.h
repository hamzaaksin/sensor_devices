#ifndef __MEDIAN_FILTER_H
#define __MEDIAN_FILTER_H

#include <stdint.h>
#include "mpu9250.h"

// Pencere boyutunu buradan merkezi olarak kontrol edebilirsin
#define MEDIAN_WINDOW 5
typedef struct {
    float buffer[MEDIAN_WINDOW];
    uint8_t index;
} MedianFilter_t;
// Fonksiyon prototipi
float filter_that(float new_val, MedianFilter_t *f);

#endif

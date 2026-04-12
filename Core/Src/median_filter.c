#include "median_filter.h"
float filter_that(float new_val, MedianFilter_t *f) {
    f->buffer[f->index] = new_val;
    f->index = (f->index + 1) % MEDIAN_WINDOW;

    float sorted[MEDIAN_WINDOW];
    for (uint8_t i = 0; i < MEDIAN_WINDOW; i++) {
        sorted[i] = f->buffer[i];
    }
    
    for (uint8_t i = 0; i < MEDIAN_WINDOW - 1; i++) {
        for (uint8_t j = 0; j < MEDIAN_WINDOW - i - 1; j++) {
            if (sorted[j] > sorted[j + 1]) {
                float temp = sorted[j];
                sorted[j] = sorted[j + 1];
                sorted[j + 1] = temp;
            }
        }
    }
    
    return sorted[MEDIAN_WINDOW / 2];
}


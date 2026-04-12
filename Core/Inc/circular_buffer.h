#ifndef __CIRCULAR_BUFFER_H
#define __CIRCULAR_BUFFER_H

#include <stdint.h>

#define CIRC_BUF_SIZE 100 

struct buf_handle_t {
    float buffer[CIRC_BUF_SIZE]; 
    uint16_t head;               
    uint16_t count;              
};


void buffer_add_value(struct buf_handle_t *p_handle, float val);
int buffer_get_value(struct buf_handle_t *p_handle, float *p_sensor_data);

#endif
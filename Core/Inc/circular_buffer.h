#ifndef __CIRCULAR_BUFFER_H
#define __CIRCULAR_BUFFER_H

#include <stdint.h>

#define CIRC_BUF_SIZE 100 // İstatistik için saklanacak veri miktarı (İstediğin gibi değiştir)

struct buf_handle_t {
    float buffer[CIRC_BUF_SIZE]; // i. Statik olarak ayrılmış alan
    uint16_t head;               // Yazılacak sıradaki yer
    uint16_t count;              // Buffer içindeki güncel veri sayısı
};

// Fonksiyon Prototipleri
void buffer_add_value(struct buf_handle_t *p_handle, float val);
int buffer_get_value(struct buf_handle_t *p_handle, float *p_sensor_data);

#endif
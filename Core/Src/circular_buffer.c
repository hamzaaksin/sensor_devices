#include "circular_buffer.h"

// Veri Ekleme: ii. Eğer dolarsa en eskiyi sil (üzerine yaz)
void buffer_add_value(struct buf_handle_t *p_handle, float val) {
    p_handle->buffer[p_handle->head] = val;
    p_handle->head = (p_handle->head + 1) % CIRC_BUF_SIZE; // Dairesel dönüş

    if (p_handle->count < CIRC_BUF_SIZE) {
        p_handle->count++;
    }
}

// iii. Ödevde istenen özel fonksiyon
// Başarılıysa 0 döner, veri yoksa -1
int buffer_get_value(struct buf_handle_t *p_handle, float *p_sensor_data) {
    if (p_handle->count == 0) {
        return -1; // Buffer boş
    }
    // En son eklenen veriyi okumak için (veya mantığa göre geliştirilebilir)
    uint16_t last_idx = (p_handle->head == 0) ? (CIRC_BUF_SIZE - 1) : (p_handle->head - 1);
    *p_sensor_data = p_handle->buffer[last_idx];
    return 0;
}
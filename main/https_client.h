#ifndef HTTPS_CLIENT_H
#define HTTPS_CLIENT_H

#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

void send_bmp_https_post(uint8_t *bmp_data, int bmp_size, int fire_value);

#ifdef __cplusplus
}
#endif

#endif // HTTPS_CLIENT_H
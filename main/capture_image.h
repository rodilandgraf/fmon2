#ifndef CAPTURE_IMAGE_H
#define CAPTURE_IMAGE_H
#include <stdint.h>

void init_thermal_camera(void);

void capture_thermal_data(float* result);

void convert_to_grayscale(float *temperatureData, uint8_t *grayImageInt, int16_t minTemp, int16_t maxTemp);

uint8_t* create_bmp_from_grayscale(const uint8_t* grayData, int* out_size);

void set_camera_refresh_rate(uint8_t rate);

void set_camera_chess_mode(void);

void set_camera_interleaved_mode(void);

#endif // CAPTURE_IMAGE_H
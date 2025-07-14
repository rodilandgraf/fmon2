#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "capture_image.h"

static const char *TAG = "FMON2_MAIN";

void mlx90640_setup(void);
uint8_t *mlx90640_picture(float *thermal_buffer, uint8_t *grayscale_buffer);

void app_main(void)
{
    static float thermal_data_array[768];
    static uint8_t grayscale_image[768];

    mlx90640_setup();
    vTaskDelay(pdMS_TO_TICKS(500));

    while (1)
    {
        uint8_t *bmp_image = NULL;
        bmp_image = mlx90640_picture(thermal_data_array, grayscale_image);

        free(bmp_image);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void mlx90640_setup(void)
{
    init_thermal_camera();
    set_camera_refresh_rate(1);
    set_camera_interleaved_mode();
}

uint8_t *mlx90640_picture(float *thermal_buffer, uint8_t *grayscale_buffer)
{
    capture_thermal_data(thermal_buffer);

    convert_to_grayscale(thermal_buffer, grayscale_buffer, 20, 300);

    int bmp_size = 0;
    uint8_t *bmp_data = create_bmp_from_grayscale(grayscale_buffer, &bmp_size);

    if (bmp_data)
    {
        ESP_LOGI(TAG, "BMP image successfully created (%d bytes).", bmp_size);
    }
    else
    {
        ESP_LOGE(TAG, "BMP image creation failed.");
    }
    return bmp_data;
}
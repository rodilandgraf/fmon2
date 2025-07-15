#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "capture_image.h"
#include "cnn_classifier.h"
#include "wifi_manager.h"
#include "https_client.h"

static const char *TAG = "FMON2_MAIN";

void app_main(void)
{
    static float thermal_data_array[768];
    static uint8_t grayscale_image[768];

    init_thermal_camera();
    set_camera_refresh_rate(1);
    set_camera_interleaved_mode();
    init_tflite_model();
    wifi_init();

    // Delay to satbilize camera
    vTaskDelay(pdMS_TO_TICKS(5000));
    ESP_LOGI(TAG, "Discarding first image");
    capture_thermal_data(thermal_data_array);
    vTaskDelay(pdMS_TO_TICKS(500));

    ESP_LOGI(TAG, "Initialization complete, starting main loop.");

    while (1)
    {
        uint8_t *bmp_image = NULL;

        bmp_image = mlx90640_picture(thermal_data_array, grayscale_image);
        int cnn_result = run_inference(grayscale_image);

        send_bmp_https_post(bmp_image, image_size(), cnn_result);

        free(bmp_image);
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "capture_image.h"

static const char *TAG = "FMON2_MAIN";

void mlx90640_setup(void);
void mlx90640_picture(void);

void app_main(void)
{
    mlx90640_setup();

    const TickType_t xDelay = pdMS_TO_TICKS(2000);

    while (1)
    {
        vTaskDelay(xDelay);
    }
}

void mlx90640_setup(void)
{

    init_thermal_camera();
    set_camera_refresh_rate(1);
    set_camera_interleaved_mode();

    static float thermal_data_array[768];
}
void mlx90640_picture(void)
{
    capture_thermal_data(thermal_data_array);

    ESP_LOGI(TAG, "--- New Thermal Frame Captured ---");
    for (int p = 0; p < 768; p++)
    {
        printf("%.2f, ", thermal_data_array[p]);
        if ((p + 1) % 32 == 0)
        {
            printf("\n");
        }
    }
}
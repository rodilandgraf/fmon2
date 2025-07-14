#include <string.h>
#include <stdlib.h>
#include "capture_image.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "MLX90640_API.h"

#define TA_SHIFT 8
#define IMAGE_WIDTH 32
#define IMAGE_HEIGHT 24

static const char *TAG = "CAPTURE_IMAGE";
static paramsMLX90640 mlx90640;
static uint8_t slaveAddress = 0x33;

void init_thermal_camera(void)
{
    ESP_LOGI(TAG, "Initializing thermal camera...");
    REDAR_I2CInit();

    uint16_t *eeMLX90640 = (uint16_t *)malloc(832 * sizeof(uint16_t));
    if (eeMLX90640 == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for EEPROM data");
        return;
    }

    int status = MLX90640_DumpEE(slaveAddress, eeMLX90640);
    if (status != 0)
    {
        ESP_LOGE(TAG, "Failed to dump EEPROM (error %d)", status);
        free(eeMLX90640);
        return;
    }

    status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
    if (status != 0)
    {
        ESP_LOGE(TAG, "Failed to extract parameters (error %d)", status);
    }

    free(eeMLX90640);
    ESP_LOGI(TAG, "Thermal camera initialized successfully.");
}

void capture_thermal_data(float *result)
{
    float emissivity = 0.3;
    float tr;

    uint16_t *mlx90640Frame = (uint16_t *)malloc(834 * sizeof(uint16_t));
    if (mlx90640Frame == NULL)
    {
        ESP_LOGE(TAG, "Failed to allocate memory for frame data");
        return;
    }

    for (int i = 0; i < 2; i++)
    {
        MLX90640_GetSubFrameData(slaveAddress, mlx90640Frame);
        tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT;
        MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, result);
    }

    free(mlx90640Frame);
}

void convert_to_grayscale(float *temperatureData, uint8_t *grayImageInt, int16_t minTemp, int16_t maxTemp)
{
    int i;
    for (i = 0; i < 768; i++)
    {
        int16_t temp = temperatureData[i];

        int norm = (temp - minTemp) * 255 / (maxTemp - minTemp);
        if (norm < 0)
            norm = 0;
        if (norm > 255)
            norm = 255;

        grayImageInt[i] = norm;
    }
}

uint8_t *create_bmp_from_grayscale(const uint8_t *grayData, int *out_size)
{
    int row_padded = (IMAGE_WIDTH + 3) & (~3);
    int pixel_array_size = row_padded * IMAGE_HEIGHT;

    int file_size = 14 + 40 + (256 * 4) + pixel_array_size;

    *out_size = file_size;
    uint8_t *bmp = (uint8_t *)malloc(file_size);
    if (!bmp)
    {
        return NULL;
    }
    memset(bmp, 0, file_size);

    bmp[0] = 'B';
    bmp[1] = 'M';
    *(uint32_t *)(bmp + 2) = file_size;
    *(uint32_t *)(bmp + 10) = 14 + 40 + (256 * 4);
    *(uint32_t *)(bmp + 14) = 40;
    *(int32_t *)(bmp + 18) = IMAGE_WIDTH;
    *(int32_t *)(bmp + 22) = IMAGE_HEIGHT;
    *(uint16_t *)(bmp + 26) = 1;
    *(uint16_t *)(bmp + 28) = 8;
    *(uint32_t *)(bmp + 34) = pixel_array_size;

    for (int i = 0; i < 256; i++)
    {
        int offset = 14 + 40 + i * 4;
        bmp[offset + 0] = i;
        bmp[offset + 1] = i;
        bmp[offset + 2] = i;
        bmp[offset + 3] = 0;
    }

    uint8_t *p = bmp + 14 + 40 + (256 * 4);

    for (int y = IMAGE_HEIGHT - 1; y >= 0; y--)
    {

        for (int x = 0; x < IMAGE_WIDTH; x++)
        {
            *p++ = grayData[y * IMAGE_WIDTH + x];
        }

        for (int i = 0; i < (row_padded - IMAGE_WIDTH); i++)
        {
            *p++ = 0;
        }
    }

    return bmp;
}

void set_camera_refresh_rate(uint8_t rate)
{
    int status = MLX90640_SetRefreshRate(slaveAddress, rate);
    if (status == 0)
    {
        ESP_LOGI(TAG, "Camera frequency set to: %d", rate);
    }
    else
    {
        ESP_LOGE(TAG, "Frequency configuration failed (error %d)", status);
    }
}

void set_camera_chess_mode(void)
{

    int status = MLX90640_SetChessMode(slaveAddress);
    if (status == 0)
    {
        ESP_LOGI(TAG, "Camera mode set to: Chess");
    }
    else
    {
        ESP_LOGE(TAG, "Mode configuration failed (error %d)", status);
    }
}

void set_camera_interleaved_mode(void)
{
    int status = MLX90640_SetInterleavedMode(slaveAddress); //
    if (status == 0)
    {
        ESP_LOGI(TAG, "Camera mode set to: Interleaved");
    }
    else
    {
        ESP_LOGE(TAG, "Mode configuration failed (error %d)", status);
    }
}
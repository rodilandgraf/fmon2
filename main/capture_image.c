#include "capture_image.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "MLX90640_API.h"

#define TA_SHIFT 8

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
    float emissivity = 0.5;
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

void set_camera_refresh_rate(uint8_t rate)
{
    if (rate > 7)
    {
        ESP_LOGW(TAG, "Valor de frequência inválido: %d. Use um valor entre 0 e 7.", rate);
        return;
    }
    int status = MLX90640_SetRefreshRate(slaveAddress, rate);
    if (status == 0)
    {
        ESP_LOGI(TAG, "Frequência da câmera configurada para o valor: %d", rate);
    }
    else
    {
        ESP_LOGE(TAG, "Falha ao configurar a frequência da câmera (erro %d)", status);
    }
}

void set_camera_chess_mode(void)
{

    int status = MLX90640_SetChessMode(slaveAddress);
    if (status == 0)
    {
        ESP_LOGI(TAG, "Modo da câmera configurado para: Chess");
    }
    else
    {
        ESP_LOGE(TAG, "Falha ao configurar o modo Chess (erro %d)", status);
    }
}

void set_camera_interleaved_mode(void)
{
    int status = MLX90640_SetInterleavedMode(slaveAddress); //
    if (status == 0)
    {
        ESP_LOGI(TAG, "Modo da câmera configurado para: Interleaved");
    }
    else
    {
        ESP_LOGE(TAG, "Falha ao configurar o modo Interleaved (erro %d)", status);
    }
}
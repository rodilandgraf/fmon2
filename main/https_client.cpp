#include "https_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include <string.h>

extern "C" {
    extern const uint8_t _binary_lse_pem_start[];
    extern const uint8_t _binary_lse_pem_end[];
}

void send_bmp_https_post(uint8_t *bmp_data, int bmp_size, int fire_value)
{
    const char *boundary = "----ESP32Boundary";
    char content_type[128];
    snprintf(content_type, sizeof(content_type), "multipart/form-data; boundary=%s", boundary);

    esp_http_client_config_t config = {
        .url = "https://lse.cp.utfpr.edu.br/fmon/post-file.php",
        .cert_pem = reinterpret_cast<const char *>(_binary_lse_pem_start),
        .transport_type = HTTP_TRANSPORT_OVER_SSL,
    };
    esp_http_client_handle_t client = esp_http_client_init(&config);

    char part1[512];
    snprintf(part1, sizeof(part1),
         "--%s\r\n"
         "Content-Disposition: form-data; name=\"api_key\"\r\n\r\n"
         "Fm#25\r\n"
         "--%s\r\n"
         "Content-Disposition: form-data; name=\"fire\"\r\n\r\n"
         "%d\r\n"
         "--%s\r\n"
         "Content-Disposition: form-data; name=\"file\"; filename=\"image.bmp\"\r\n"
         "Content-Type: image/bmp\r\n\r\n",
         boundary, boundary, fire_value, boundary);

    const char *part3_format = "\r\n--%s--\r\n";
    char part3[64];
    snprintf(part3, sizeof(part3), part3_format, boundary);

    int total_len = strlen(part1) + bmp_size + strlen(part3);

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", content_type);
    esp_http_client_set_header(client, "Content-Length", (const char *)NULL);
    esp_http_client_open(client, total_len);

    esp_http_client_write(client, part1, strlen(part1));
    esp_http_client_write(client, (char *)bmp_data, bmp_size);
    esp_http_client_write(client, part3, strlen(part3));

    int status_code = esp_http_client_fetch_headers(client);
    if (status_code > 0)
    {
        ESP_LOGI("HTTP", "Status code: %d", esp_http_client_get_status_code(client));
    }
    else
    {
        ESP_LOGE("HTTP", "Falha ao enviar dados. Código: %d", status_code);
    }
    esp_http_client_cleanup(client);
}

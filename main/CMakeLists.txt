idf_component_register(SRCS "fmon2.c" "capture_image.c" "cnn_classifier.cpp" "wifi_manager.c" "https_client.cpp"
                     EMBED_TXTFILES "lse.pem"
                      REQUIRES mlx90640_redar
                              nvs_flash
                               esp_netif
                               esp_event
                               esp_wifi
                               esp_http_client
                               )

idf_component_register(SRCS "MLX90640_API.c" "redar_i2c_driver.c"
                      
                      INCLUDE_DIRS "."
                     
                      REQUIRES driver freertos log)
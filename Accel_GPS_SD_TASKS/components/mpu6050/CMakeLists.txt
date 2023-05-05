idf_component_register(SRCS "mpu6050.cpp"
                       # INCLUDE_DIRS "include"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls
                       REQUIRES "driver"
                       REQUIRES esp32-i2c_rw)
idf_component_register(SRCS "SDcard.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls
                       REQUIRES "driver"
                       REQUIRES fatfs
                       REQUIRES vfs
                       REQUIRES sdmmc)

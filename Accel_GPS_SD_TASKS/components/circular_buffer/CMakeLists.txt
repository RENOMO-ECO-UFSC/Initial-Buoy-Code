idf_component_register(SRCS "circbuf"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls
                       REQUIRES "driver"
                       REQUIRES fatfs
                       REQUIRES vfs
                       REQUIRES sdmmc)

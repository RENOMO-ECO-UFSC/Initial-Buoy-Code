idf_component_register(SRCS "sim800l.c"
                       INCLUDE_DIRS "include"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls
                       REQUIRES "driver"
                       )
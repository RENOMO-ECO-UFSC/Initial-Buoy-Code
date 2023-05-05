idf_component_register(SRCS "ublox7.cpp"
                       INCLUDE_DIRS "include"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls
                       REQUIRES "driver"
                       )
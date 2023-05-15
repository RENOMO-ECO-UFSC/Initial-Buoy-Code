idf_component_register(SRCS "ring_buffer.cpp"
                       INCLUDE_DIRS "."
                       REQUIRES mbedtls
                       REQUIRES "driver"
)

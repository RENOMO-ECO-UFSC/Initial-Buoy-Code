idf_component_register(SRCS "cppuart.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES mbedtls
                    REQUIRES "driver")

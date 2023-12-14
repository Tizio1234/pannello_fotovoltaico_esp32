idf_component_register(SRCS     "main.c"
                                "./src/ldr_adc.c"
                                "./src/servo.c"
                INCLUDE_DIRS    "."
                                "./inc"
                REQUIRES        servo
                                esp_adc
                                freertos)
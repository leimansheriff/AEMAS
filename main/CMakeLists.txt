idf_component_register(SRCS "AEMAS.c"
					   INCLUDE_DIRS "." "include"
					   REQUIRES esp_driver_gpio freertos log mk3 led usbconf wifi server)


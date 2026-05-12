idf_component_register(SRCS "usbconf.c"
                    INCLUDE_DIRS "include"
					REQUIRES "usb" "usb_host_cdc_acm"
					PRIV_REQUIRES "mk3" usb_host_ftdi_vcp)

idf_component_register(SRCS "server.c"
                    INCLUDE_DIRS "include"
					REQUIRES esp_http_server 
					PRIV_REQUIRES mk3 cjson)
target_add_binary_data(${COMPONENT_LIB} "index.html" TEXT)

set(SRC 
       "epd.c"
       "gui.c"
       "ntp.c"
       "dht.c"
       "main.c"
       "news.c"
       "scrn.c"
       "wifi.c"
       "stock.c"
   )

idf_component_register(SRCS ${SRC}
                       INCLUDE_DIRS ".")

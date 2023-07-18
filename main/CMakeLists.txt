idf_component_register(
    SRCS 
    "./mqtt/mqtt.c"
    "./wifi/wifi.c"
    "app_main.c"
    "./sensores/luzQuarto/luzQuarto.c"
    "./sensores/ligaTela/ligaTela.c"
    "./mqtt/json_parser.c"
                    
    INCLUDE_DIRS 
    "."
    "./mqtt/"
    "./wifi/"
    "./sensores/ligaTela/"
    "./sensores/luzQuarto/"
)
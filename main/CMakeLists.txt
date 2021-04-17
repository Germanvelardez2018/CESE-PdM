


# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS 
"main.c"
"ksi_serial.c"
"ksi_memory_store.c"
"command_processor.c")
set(COMPONENT_ADD_INCLUDEDIRS "./inc")

register_component()

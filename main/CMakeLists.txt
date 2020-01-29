# Edit following two lines to set component requirements (see docs)
set(COMPONENT_SRCS "main.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "./Wi-Fi" "./HardwareController" "./Socket" "./Virtual_Devices")

add_definitions("-frtti")

register_component()

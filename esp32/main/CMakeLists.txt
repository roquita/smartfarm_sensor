file(GLOB_RECURSE C_SOURCES *.c)
file(GLOB_RECURSE CPP_SOURCES *.cpp)

idf_component_register(SRCS ${C_SOURCES}  ${CPP_SOURCES} 
                    INCLUDE_DIRS ".")

set(WEB_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../front") 
spiffs_create_partition_image(www ${WEB_SRC_DIR} FLASH_IN_PROJECT)    
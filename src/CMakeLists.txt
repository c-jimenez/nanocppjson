
# nanojsoncpp library
add_library(nanojsoncpp
    nanojsoncpp.cpp
    nanojsoncpp_gen.cpp
    nanojsoncpp_parse.cpp
    nanojsoncpp_utils.cpp
)

target_include_directories(nanojsoncpp PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
    ${NANOJSONCPP_CFG_PATH}
)
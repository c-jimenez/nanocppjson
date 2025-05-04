
# nanojsoncpp library
add_library(nanojsoncpp
    nanojsoncpp.cpp
    utils.cpp
)

target_include_directories(nanojsoncpp PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/../inc
)
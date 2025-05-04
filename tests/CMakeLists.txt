
# Common test libraries
set(NANOJSONCPP_COMMON_TEST_LIBS nanojsoncpp doctest)

# Utilities
add_executable(nanojsoncpp_parse
  nanojsoncpp_parse.cpp
)
target_link_libraries(nanojsoncpp_parse nanojsoncpp)

# Unit tests
add_executable(test_optional 
  test_optional.cpp 
)
target_link_libraries(test_optional ${NANOJSONCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_optional
  COMMAND test_optional
)

add_executable(test_parse 
  test_parse.cpp 
)
target_link_libraries(test_parse ${NANOJSONCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_parse
  COMMAND test_parse
)

add_executable(test_utils 
  test_utils.cpp 
)
target_link_libraries(test_utils ${NANOJSONCPP_COMMON_TEST_LIBS})
add_test(
  NAME test_utils
  COMMAND test_utils
)


if (${BUILD_PERF_TESTS})

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(test_perfs 
  test_perfs.cpp
  ${CMAKE_SOURCE_DIR}/3rdparty/tiny-json/tiny-json.c
)
target_include_directories(test_perfs PRIVATE ${CMAKE_SOURCE_DIR}/3rdparty/tiny-json/)
target_compile_options(test_perfs PRIVATE -Wno-sign-conversion)
target_compile_definitions(test_perfs PRIVATE TEST_FILES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/files")
target_link_libraries(test_perfs ${NANOJSONCPP_COMMON_TEST_LIBS} nlohmann_json::nlohmann_json)
add_test(
  NAME test_perfs
  COMMAND test_perfs
)

endif()

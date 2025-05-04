
# Doctest : Unit test library, used in single header mode
add_library(doctest INTERFACE)
target_include_directories(doctest INTERFACE doctest-2.4.11/doctest)

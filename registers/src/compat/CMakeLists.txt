add_library(compat barrier.cpp)
target_link_libraries(compat project_options)
target_include_directories(compat PRIVATE ${CMAKE_SOURCE_DIR}/src)

project(registers)
cmake_minimum_required(VERSION 3.17)

find_package( Threads )

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_options(project_options
                       INTERFACE
					   	-fsanitize=thread
					   	-Wall -Werror -Wpedantic -Wextra)
target_link_libraries(project_options
                      INTERFACE -fsanitize=thread)

add_library(registers src/smrswb_register.cpp src/thread_id.cpp)
target_link_libraries(registers project_options)
target_include_directories(registers PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(driver driver.m.cpp)
target_link_libraries(driver PRIVATE project_options registers ${CMAKE_THREAD_LIBS_INIT})

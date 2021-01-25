project(registers)
cmake_minimum_required(VERSION 3.17)

find_package(Threads)

option(ENABLE_THREAD_SANITIZER "Enable thread sanitizer" ON)
option(ENABLE_UB_SANITIZER "Enable undefined behaviour sanitizer" ON)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)
target_compile_options(project_options INTERFACE -Wall -Werror -Wpedantic
                                                 -Wextra -g -O3)
# target_link_libraries(project_options INTERFACE)

set(SANITIZERS "")
if(ENABLE_THREAD_SANITIZER)
    list(APPEND SANITIZERS "thread")
endif()
if(ENABLE_UB_SANITIZER)
    list(APPEND SANITIZERS "undefined")
endif()

list(JOIN SANITIZERS "," SANITIZERS_LIST)

if(SANITIZERS_LIST)
    if(NOT "${SANITIZERS_LIST}" STREQUAL "")
        target_compile_options(project_options
            					INTERFACE -fsanitize=${SANITIZERS_LIST})
        target_link_libraries(project_options
            					INTERFACE -fsanitize=${SANITIZERS_LIST})
    endif()
endif()

add_subdirectory(src/compat)

add_library(registers src/smrswb_register.cpp src/thread_id.cpp)
target_link_libraries(registers project_options)
target_include_directories(registers PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(driver driver.m.cpp)
target_link_libraries(driver PRIVATE project_options registers compat
                                     ${CMAKE_THREAD_LIBS_INIT})

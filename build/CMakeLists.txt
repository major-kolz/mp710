# Для сборки проекта перейдите в директорию с данным файлом
#   и выполните в командной строке сначала 'cmake .'
#   а потом 'make'
# Вы также можете собрать проект под Code::Blocks, выполнив 
#   'cmake . --DCMAKE_BUILD_TYPE=Debug' - это сформирует проект под эту IDE

cmake_minimum_required(VERSION 2.8.7)  # Поверка версии cmake
project(mp710_console)

# Определяем флаги сборки
set(DEBUG_FLAGS "-g -Og -pipe -Wall -pedantic -std=c99 -pthread")
set(RELEASE_FLAGS "-O2 -pipe -march=native -std=c99 -pthread")
set(EXECUTABLE_OUTPUT_PATH ../bin)

# Определяем параметры сборки
set(prog_name mp710_t)
set(CMAKE_C_FLAGS ${RELEASE_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable( ${prog_name} ../main.c ../hid.c ../hidapi.h)
target_link_libraries( ${prog_name} rt usb-1.0 )

IF(WIN32) # Check if we are on Windows
	message(SEND_ERROR "This code will not work on Windows platform")
ENDIF()

# 'make run' - собрать и запустить
add_custom_target(run
    COMMAND ${prog_name}
    DEPENDS ${prog_name}
    WORKING_DIRECTORY ../bin
)

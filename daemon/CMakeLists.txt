cmake_minimum_required(VERSION 2.8.7)  # Поверка версии cmake
project(Daemon)

# Определяем флаги сборки
set(DEBUG_FLAGS "-g -Og -pipe -Wall -pedantic -std=gnu99 -pthread")
set(RELEASE_FLAGS "-O2 -pipe -march=native -std=gnu99 -pthread")
set(EXECUTABLE_OUTPUT_PATH ../bin)

# Определяем параметры сборки
set(prog_name mp710_daemon)
set(CMAKE_C_FLAGS ${RELEASE_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CONFIGURATION_TYPES Debug Release)

set(headers ./include)
set(sources ./src)
file(GLOB code 
	${sources}/*.c	
)
# Необходимо явно подключить заголовочные файлы инструкцией add_executable
# 	в случае, если они лежат не в той же папке, что и CMakeLists.txt
file(GLOB code_h
	${headers}/*.h
)


include_directories(${headers})

add_executable( ${prog_name} ${code} ${code_h})
target_link_libraries( ${prog_name} rt usb-1.0 pthread)

IF(WIN32) # Check if we are on Windows
	message(SEND_ERROR "This code will not work on Windows platform")
ENDIF()

# 'make run' - собрать и запустить
add_custom_target(run_daemon
    COMMAND ${prog_name}
    DEPENDS ${prog_name}
    WORKING_DIRECTORY ../bin
)

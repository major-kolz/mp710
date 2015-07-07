cmake_minimum_required(VERSION 2.8.7)  # Поверка версии cmake
project(Controller)

# Определяем флаги сборки
set(DEBUG_FLAGS "-g -Og -pipe -Wall -pedantic -std=gnu++11")
set(RELEASE_FLAGS "-O2 -pipe -std=gnu++11 -march=native")
set(EXECUTABLE_OUTPUT_PATH ../../bin)

# Определяем параметры сборки
set(prog_name mp710_gui)
set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Переменные для сборки проекта
set(headers ../include)
set(resources ../res)
set(sources ../src)
file(GLOB code 
	${sources}/*.cpp	
)
# Необходимо явно подключить заголовочные файлы инструкцией add_executable
# 	в случае, если они лежат не в той же папке, что и CMakeLists.txt
file(GLOB code_h
	${headers}/*.h
)

include_directories(${headers} ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5Widgets REQUIRED)
qt5_wrap_ui(wrapped_ui ${resources}/controller.ui)
# The Qt5Widgets_INCLUDES also includes the include directories for
# dependencies QtCore and QtGui
include_directories(${Qt5Widgets_INCLUDES})

# We need add -DQT_WIDGETS_LIB when using QtWidgets in Qt 5.
add_definitions(${Qt5Widgets_DEFINITIONS})

# Executables fail to build with Qt 5 in the default configuration
# without -fPIE. We add that here.
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

add_executable( ${prog_name} ${code} ${code_h} ${wrapped_ui} )
# The Qt5Widgets_LIBRARIES variable also includes QtGui and QtCore
target_link_libraries(${prog_name} ${Qt5Widgets_LIBRARIES})

IF(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(${prog_name} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
  else()
    message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
  endif()
elseif(UNIX)
  # Nothing special required
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()

# 'make run' - собрать и запустить
add_custom_target(run
    COMMAND ${prog_name}
    DEPENDS ${prog_name}
    WORKING_DIRECTORY ../../bin
)

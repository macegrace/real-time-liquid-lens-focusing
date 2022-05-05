cmake_minimum_required(VERSION 3.10)

project(AutoFocusNonEmbedded)

set(OpenCV_DIR /installation/OpenCV-/lib/cmake/opencv4)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

find_package(OpenCV REQUIRED)

set(SOURCES_IDS
    src/AutoFocusNonEmbedded.cpp
    src/Flexiboard.cpp
    src/SerialPort.cpp
    src/CamControl.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES_IDS})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(${PROJECT_NAME} PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(${PROJECT_NAME} "${CMAKE_THREAD_LIBS_INIT}")
endif()

include_directories(include)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

target_link_libraries(${PROJECT_NAME} /opt/ids/ueye/lib/libueye_api.so)

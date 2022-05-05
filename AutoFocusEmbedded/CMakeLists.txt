cmake_minimum_required(VERSION 3.10)

project(AutoFocusEmbedded)

set(OpenCV_DIR /installation/OpenCV-/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

set(SOURCES_AUTOFOCUS
    src/AutoFocusEmbedded.cpp
    src/Trigger.cpp
    src/VideoCapture.cpp
    src/ADM00931.cpp
    src/IFocusAlgorithm.cpp
    src/FFTfocus.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES_AUTOFOCUS})

include_directories(include)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})

find_library(GPIOD_LIBRARY NAMES libgpiod.so)
target_link_libraries(${PROJECT_NAME} ${GPIOD_LIBRARY})

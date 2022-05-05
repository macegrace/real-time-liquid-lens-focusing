cmake_minimum_required(VERSION 3.10)

project(TestSuite)

set(TEST_SETS TestSetsGenerator)
set(AUTOFOCUS_NO_CAM AutoFocusNoCam)
set(OpenCV_DIR /installation/OpenCV-/lib/cmake/opencv4)

find_package(OpenCV REQUIRED)

set(SOURCES_TEST_SETS
    src/TestSetsGenerator.cpp
    src/Trigger.cpp
    src/VideoCapture.cpp
    src/ADM00931.cpp
)

set(SOURCES_AUTOFOCUS_NO_CAM
    src/AutoFocusNoCam.cpp
)

add_executable(${TEST_SETS} ${SOURCES_TEST_SETS})
add_executable(${AUTOFOCUS_NO_CAM} ${SOURCES_AUTOFOCUS_NO_CAM})

include_directories(include)
target_link_libraries(${TEST_SETS} ${OpenCV_LIBS})
target_link_libraries(${AUTOFOCUS_NO_CAM} ${OpenCV_LIBS})

find_library(GPIOD_LIBRARY NAMES libgpiod.so)
target_link_libraries(${TEST_SETS} ${GPIOD_LIBRARY})
target_link_libraries(${AUTOFOCUS_NO_CAM} ${GPIOD_LIBRARY})

target_compile_options(${AUTOFOCUS_NO_CAM} PRIVATE -Og -ggdb3 -g3 -fno-omit-frame-pointer)

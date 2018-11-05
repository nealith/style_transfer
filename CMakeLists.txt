CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(StyleTransfert)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS}  ${StyleTransfert_SOURCE_DIR}/includes)
file(GLOB SOURCES
    ./includes/*.h
    ./src/*.cpp
)

ADD_EXECUTABLE(style_transfert main.cpp ${SOURCES})

target_link_libraries( style_transfert ${OpenCV_LIBS} )

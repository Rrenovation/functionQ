cmake_minimum_required(VERSION 3.5)

project(functionQ LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

add_executable(functionQ
  main.cpp
  functionQ.h
  functionQ.cpp
  ./scrcpy/decoder/tcpserver.h
  ./scrcpy/decoder/tcpserver.cpp
  ./scrcpy/decoder/videosocket.h
  ./scrcpy/decoder/videosocket.cpp
  ./scrcpy/decoder/stream.h
  ./scrcpy/decoder/stream.cpp
  ./scrcpy/decoder/videobuffer.h
  ./scrcpy/decoder/videobuffer.cpp
  ./scrcpy/decoder/decoder.h
  ./scrcpy/decoder/decoder.cpp
  ./scrcpy/decoder/avframeconvert.h
  ./scrcpy/decoder/avframeconvert.cpp
  ./scrcpy/decoder/adbprocess.h
  ./scrcpy/decoder/adbprocess.cpp
  ./scrcpy/controller/qscrcpyevent.h
  ./scrcpy/controller/controlmsg.h
  ./scrcpy/controller/controlmsg.cpp
  ./scrcpy/controller/bufferutil.h
  ./scrcpy/controller/bufferutil.cpp
  ./scrcpy/controller/controller.h
  ./scrcpy/controller/controller.cpp
  ./scrcpy/controller/action.h
  ./scrcpy/controller/action.cpp
)
target_link_libraries(functionQ Qt5Core Qt5::Network avcodec avformat avutil swscale )
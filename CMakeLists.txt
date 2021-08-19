cmake_minimum_required(VERSION 3.5)

project(functionQ LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Core Network REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR} SrcList)
aux_source_directory(${PROJECT_SOURCE_DIR}/scrcpy DeviceList)
aux_source_directory(${PROJECT_SOURCE_DIR}/scrcpy/decoder DecoderList)
aux_source_directory(${PROJECT_SOURCE_DIR}/scrcpy/controller ControllerList)

add_executable(${PROJECT_NAME}
  ${SrcList}
  ${DeviceList}
  ${DecoderList}
  ${ControllerList}
)
target_link_libraries(functionQ Qt5Core Qt5::Network avcodec avformat avutil swscale )
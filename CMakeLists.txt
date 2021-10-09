cmake_minimum_required(VERSION 3.5)

project(functionQ LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(build_library OFF)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
  set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKR_FLAGS} /SAFESEH:NO")
  # add_compile_options(
  #   $<$<CONFIG:>:/MT> #---------|
  #   $<$<CONFIG:Debug>:/MTd> #---|-- Statically link the runtime libraries
  #   $<$<CONFIG:Release>:/MT> #--|
  # )
  # set(Qt5_DIR E:\\qt5_static\\lib\\cmake\\Qt5) Statically link patch
	include_directories(${PROJECT_SOURCE_DIR}\\thrid_party\\ffmpeg\\include)
	link_directories(${PROJECT_SOURCE_DIR}\\thrid_party\\ffmpeg\\lib)
  
endif()

find_package(Qt5 COMPONENTS Core Network REQUIRED)

aux_source_directory(${PROJECT_SOURCE_DIR} SrcList)
aux_source_directory(${PROJECT_SOURCE_DIR}/scrcpy DeviceList)
aux_source_directory(${PROJECT_SOURCE_DIR}/scrcpy/decoder DecoderList)
aux_source_directory(${PROJECT_SOURCE_DIR}/scrcpy/controller ControllerList)


if(build_library)
  SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}\\build)

  add_library(${PROJECT_NAME} SHARED
    ${DeviceList}
    ${DecoderList}
    ${ControllerList}
  )
else()
  add_executable(${PROJECT_NAME} 
  ${SrcList}
  ${DeviceList}
  ${DecoderList}
  ${ControllerList}
)
endif()


target_link_libraries(functionQ Qt5::Core Qt5::Network avcodec avformat avutil swscale )
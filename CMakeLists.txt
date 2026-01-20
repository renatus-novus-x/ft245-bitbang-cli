cmake_minimum_required(VERSION 3.6)

project(ft245-bitbang-cli LANGUAGES CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

file(GLOB_RECURSE SRC "src/*.cpp")
add_executable(ft245-bitbang-cli ${SRC}
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ft245-bitbang-cli)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
set(CMAKE_SUPPRESS_REGENERATION true)

if(MSVC)
  add_definitions(-D_UNICODE -DUNICODE -D_USE_MATH_DEFINES -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")
  set_target_properties(ft245-bitbang-cli PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$(ProjectDir)/../bin")

  set(CompilerFlags
    CMAKE_CXX_FLAGS
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
    CMAKE_C_FLAGS
    CMAKE_C_FLAGS_DEBUG
    CMAKE_C_FLAGS_RELEASE
  )
  #foreach(CompilerFlag ${CompilerFlags})
  #  string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  #endforeach()

add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/bin"
)
endif(MSVC)

# User can pass: -DFTDI_D2XX_ROOT=/path/to/d2xx
set(FTDI_D2XX_ROOT "" CACHE PATH "Path to FTDI D2XX (contains ftd2xx.h and libraries)")

find_path(FTD2XX_INCLUDE_DIR
  NAMES ftd2xx.h
  HINTS ${FTDI_D2XX_ROOT} ENV FTDI_D2XX_ROOT
  PATH_SUFFIXES include
)

find_library(FTD2XX_LIBRARY
  NAMES ftd2xx libftd2xx
  HINTS ${FTDI_D2XX_ROOT} ENV FTDI_D2XX_ROOT
  PATH_SUFFIXES lib lib64 amd64 i386
)

if(NOT FTD2XX_INCLUDE_DIR OR NOT FTD2XX_LIBRARY)
  message(FATAL_ERROR
    "FTDI D2XX not found.\n"
    "Set -DFTDI_D2XX_ROOT to the extracted D2XX package directory.\n"
    "Need: ftd2xx.h and (ftd2xx.lib or libftd2xx.*)"
  )
endif()

target_include_directories(ft245-bitbang-cli PRIVATE ${FTD2XX_INCLUDE_DIR})
target_link_libraries(ft245-bitbang-cli PRIVATE ${FTD2XX_LIBRARY})

# Linux often needs these when linking proprietary .so
if(UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(ft245-bitbang-cli PRIVATE Threads::Threads)
  find_library(DL_LIB dl)
  if(DL_LIB)
    target_link_libraries(ft245-bitbang-cli PRIVATE ${DL_LIB})
  endif()
  find_library(RT_LIB rt)
  if(RT_LIB)
    target_link_libraries(ft245-bitbang-cli PRIVATE ${RT_LIB})
  endif()
endif()


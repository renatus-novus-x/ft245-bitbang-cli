cmake_minimum_required(VERSION 3.16)
project(ft245_bitbang_cli LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ft245_bitbang_cli
  src/main.cpp
  src/platform.h
  src/platform_win.cpp
  src/platform_posix.cpp
)

# Produce a hyphenated binary name.
set_target_properties(ft245_bitbang_cli PROPERTIES OUTPUT_NAME "ft245-bitbang-cli")

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

target_include_directories(ft245_bitbang_cli PRIVATE ${FTD2XX_INCLUDE_DIR})
target_link_libraries(ft245_bitbang_cli PRIVATE ${FTD2XX_LIBRARY})

# Linux often needs these when linking proprietary .so
if(UNIX AND NOT APPLE)
  find_package(Threads REQUIRED)
  target_link_libraries(ft245_bitbang_cli PRIVATE Threads::Threads)
  find_library(DL_LIB dl)
  if(DL_LIB)
    target_link_libraries(ft245_bitbang_cli PRIVATE ${DL_LIB})
  endif()
  find_library(RT_LIB rt)
  if(RT_LIB)
    target_link_libraries(ft245_bitbang_cli PRIVATE ${RT_LIB})
  endif()
endif()

# On Windows, optionally copy the DLL next to the executable if found under root
if(WIN32)
  find_file(FTD2XX_DLL
    NAMES ftd2xx.dll FTD2XX.dll
    HINTS ${FTDI_D2XX_ROOT} ENV FTDI_D2XX_ROOT
    PATH_SUFFIXES . bin lib amd64 i386
  )
  if(FTD2XX_DLL)
    add_custom_command(TARGET ft245_bitbang_cli POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${FTD2XX_DLL}" "$<TARGET_FILE_DIR:ft245_bitbang_cli>"
    )
  endif()
endif()

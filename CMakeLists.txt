cmake_minimum_required(VERSION 3.11)
project (consoleCAS-SL1)

execute_process (COMMAND date +%Y%m%d OUTPUT_VARIABLE CMAKE_BUILD_DATE OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process (COMMAND git rev-parse --short HEAD OUTPUT_VARIABLE CMAKE_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE)

set (VERSION_MAJOR 1)
set (VERSION_MINOR 2)
set (BUILD_DATE ${CMAKE_BUILD_DATE})
set (REVISION ${CMAKE_REVISION})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

configure_file (
  "${PROJECT_SOURCE_DIR}/version.h.in"
  "${PROJECT_BINARY_DIR}/version.h"
)

include_directories(${PROJECT_BINARY_DIR})

configure_file (
  "${PROJECT_SOURCE_DIR}/app/waksman.rul"
  "${PROJECT_BINARY_DIR}/app/waksman.rul"
  COPYONLY
)
configure_file (
  "${PROJECT_SOURCE_DIR}/app/balzer.rul"
  "${PROJECT_BINARY_DIR}/app/balzer.rul"
  COPYONLY
)
configure_file (
  "${PROJECT_SOURCE_DIR}/app/mazoyer.rul"
  "${PROJECT_BINARY_DIR}/app/mazoyer.rul"
   COPYONLY
)

configure_file (
  "${PROJECT_SOURCE_DIR}/app/mazoyer.rul"
  "${PROJECT_BINARY_DIR}/test/mazoyer.rul"
  COPYONLY
)

enable_testing()

add_subdirectory (app)
add_subdirectory (lib)
add_subdirectory (test)

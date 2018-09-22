cmake_minimum_required(VERSION 3.11)
project (consoleCAS-SL1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -g")

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

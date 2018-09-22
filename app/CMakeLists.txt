add_executable(cas main.c)
target_link_libraries(cas cassl1-index)
target_include_directories(cas PUBLIC ${CMAKE_SOURCE_DIR}/lib)

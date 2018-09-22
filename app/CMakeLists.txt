add_executable(cas main.c simple-view.c)
target_link_libraries(cas cassl1-index)
target_include_directories(cas PUBLIC ${CMAKE_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR})

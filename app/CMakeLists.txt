add_executable(cas main.c simple-view.c curses-view.c status-window.c cell-window.c)
target_link_libraries(cas cassl1-index curses m pthread)
target_include_directories(cas PUBLIC ${CMAKE_SOURCE_DIR}/lib ${PROJECT_SOURCE_DIR})

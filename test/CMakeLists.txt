find_package(GTest REQUIRED)
include(GoogleTest)

if (GTest_FOUND)
  add_executable(test_cas test_cas.cpp mazoyer.cpp)
  target_include_directories(test_cas PUBLIC ${CMAKE_SOURCE_DIR}/lib)
  include_directories(${GTEST_INCLUDE_DIRS})
  #link_directories(/usr/local/lib/)
  target_link_libraries(test_cas cassl1-index ${GTEST_LIBRARIES} pthread)

  gtest_add_tests(TARGET test_cas)
endif (GTest_FOUND)

# 備忘録
# make test ARGS='--output-on-failure'
# cmake --help-module-list
# cmake --help-module GoogleTest

# TODO: ExternalProject として build ディレクトリに含めることを検討

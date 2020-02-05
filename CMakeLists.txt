cmake_minimum_required (VERSION 3.5)
project(lockfree VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/cmake
    )

include("common")

#TESTING
add_executable(run_tests_fifo "alltests_fifo.cpp" )
include_directories(run_tests_fifo ${GTEST_INCLUDE_DIRS} ${GBENCH_INCLUDE_DIRS})
target_link_libraries(run_tests_fifo ${GTEST_BOTH_LIBRARIES} ${GBENCH_BOTH_LIBRARIES} pthread)
add_test(run_tests_fifo "" AUTO)

#BENCHMARKING
add_executable(run_benchmarks_fifo "allbench_fifo.cpp")
include_directories(run_benchmarks_fifo ${GBENCH_INCLUDE_DIRS})
target_link_libraries(run_benchmarks_fifo ${GBENCH_BOTH_LIBRARIES} pthread)

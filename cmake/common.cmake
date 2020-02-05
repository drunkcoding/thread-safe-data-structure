
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "-O3")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_DATA_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/data)


#Tests and grading

include(CTest)
find_package(GTest)#comes with CMake
if(GTEST_FOUND)

else(GTEST_FOUND)
	message(FATAL_ERROR "DANGER: Could not find GoogleTest")
endif(GTEST_FOUND)

#Benchmarking
find_package(GBench)
if(GBENCH_FOUND)

else(GBENCH_FOUND)
	message(FATAL_ERROR "DANGER: Could not find GoogleBenchmark")
endif(GBENCH_FOUND)

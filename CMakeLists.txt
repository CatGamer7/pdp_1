cmake_minimum_required(VERSION 3.10.0)
set (CMAKE_CXX_STANDARD 17)
project(PDP_1 VERSION 0.1.0 LANGUAGES CXX)

set(OSBitness 32)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(OSBitness 64)
endif()
set (FullOutputDir ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}${OSBitness}/${CMAKE_BUILD_TYPE})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${FullOutputDir}")


file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/source/*.cpp)
add_executable(pdp ${SOURCES})
target_include_directories(pdp PUBLIC ${CMAKE_SOURCE_DIR}/header)

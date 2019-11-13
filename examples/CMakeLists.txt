cmake_minimum_required(VERSION 2.8.7)
project(gnugraph)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
	add_definitions(/bigobj)
endif()

mark_as_advanced (CMAKE_CONFIGURATION_TYPES)
mark_as_advanced (CMAKE_INSTALL_PREFIX)

file(GLOB_RECURSE srcs ../gnugraph/*.h src/*.cpp)

include_directories(../../gnugraph)

add_executable(${PROJECT_NAME} ${srcs})
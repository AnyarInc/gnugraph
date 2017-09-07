cmake_minimum_required(VERSION 2.8.7)
project(gnugraph)

if(MSVC)
	add_definitions(/bigobj)
else()
	set(DEBUG_FLAGS "-std=c++14")
	set(RELEASE_FLAGS "-std=c++14")

	set(CMAKE_CXX_FLAGS ${RELEASE_FLAGS})
	set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
endif()

mark_as_advanced (CMAKE_CONFIGURATION_TYPES)
mark_as_advanced (CMAKE_INSTALL_PREFIX)

file(GLOB_RECURSE srcs ../gnugraph/*.h src/*.cpp)

include_directories(../../gnugraph)

add_executable(${PROJECT_NAME} ${srcs})
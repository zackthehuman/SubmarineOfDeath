cmake_minimum_required(VERSION 2.6)
project(SubmarineOfDeath)

file(GLOB SOURCE_FILES "src/*.cpp")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(SFML 2 COMPONENTS graphics audio window system REQUIRED)

include_directories(
	${SFML_INCLUDE_DIRS}
)

add_executable(SubmarineOfDeath ${SOURCE_FILES})

target_link_libraries(
	SubmarineOfDeath
	${SFML_LIBRARIES}
)

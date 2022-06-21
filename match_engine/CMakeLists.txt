cmake_minimum_required(VERSION 2.8)
project(MatchEngine)

## Compile as C++17
add_compile_options(-std=c++17)


# Locate GTest
set (GTEST_ROOT ${CMAKE_SOURCE_DIR}/ExternalLibs/gTest)
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(Project_Include_DIRS /include )

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
 include
  ${Project_Include_DIRS}
)

# Link runTests with what we want to test and the GTest and pthread library
add_executable(executeTests 
test/test.cpp
include/test.h
src/matchEngine.cpp
include/matchEngine.h
)

target_link_libraries(executeTests ${GTEST_LIBRARIES} pthread)

add_executable(Match_Engine
src/main.cpp
src/matchEngine.cpp
include/matchEngine.h
)



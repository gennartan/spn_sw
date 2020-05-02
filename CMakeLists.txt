cmake_minimum_required(VERSION 3.5)

# set the project name and version
project(Tutorial VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_STACK_SIZE "20000000")

add_subdirectory(src)
add_subdirectory(bfp)
add_subdirectory(fp)

add_executable(exe main.cpp)

# target_link_libraries(exe PUBLIC bfp)
target_link_libraries(exe PUBLIC myFloat)
target_link_libraries(exe PUBLIC SPN)
target_link_libraries(exe PUBLIC Posit)

target_include_directories(exe PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/bfp/lib"
	"${PROJECT_SOURCE_DIR}/fp/lib"
)

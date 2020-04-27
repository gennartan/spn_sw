add_library(Utils utils.cpp)
add_library(SPN spn.cpp)

target_link_libraries(Utils PUBLIC Posit)

target_link_libraries(SPN PUBLIC Utils)

target_include_directories(SPN PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/src"
	"${PROJECT_SOURCE_DIR}/bfp/lib"
)
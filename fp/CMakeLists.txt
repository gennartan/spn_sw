add_library(myFloat lib/myFloat.cpp)
add_library(Util_fp lib/util.c)
add_library(Pack_fp lib/pack.c)
add_library(OP2_fp lib/op2.c)

target_link_libraries(myFloat PUBLIC Pack_fp)
target_link_libraries(myFloat PUBLIC OP2_fp)

target_link_libraries(OP2_fp PUBLIC Util_fp)
target_link_libraries(Pack_fp PUBLIC Util_fp)


target_include_directories(myFloat PUBLIC
	"${PROJECT_SOURCE_DIR}/fp/lib"
)

# target_include_directories(myFloat PUBLIC
# 	"${PROJECT_BINARY_DIR}"
# 	"${PROJECT_SOURCE_DIR}/src"
# 	"${PROJECT_SOURCE_DIR}/fp/lib"
# )
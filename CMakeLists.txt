cmake_minimum_required(VERSION 3.10)
project(arg-parser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wfloat-equal -Wcast-align -Wstrict-prototypes -Werror-implicit-function -declaration -Wfloat-equal -Wcast-align -Wstrict-prototypes -Werror-implicit-function-declaration -Warray-bounds -Wdiv-by-zero -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=bounds -Wshadow -D_FORTIFY_SOURCE=0 -fsanitize=undefined -fno-sanitize-recover=all -Wformat=2 -DLOCAL -std=gnu++2b")

set(SRC bin/main.cpp)
add_executable(${PROJECT_NAME} bin/main.cpp)
add_subdirectory(lib)
#add_subdirectory(tests)

target_link_libraries(${PROJECT_NAME} LIB_FILES)
target_include_directories(LIB_FILES PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
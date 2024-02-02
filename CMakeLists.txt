cmake_minimum_required(VERSION 3.20)
project(arg_parser)

set(CMAKE_CXX_STANDARD 23)

include_directories(lib)
add_subdirectory(bin)
add_subdirectory(lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra  -Wall -Wextra -Wfloat-equal -Wcast-align -Wstrict-prototypes -Werror-implicit-function-declaration -Wfloat-equal -Wcast-align -Wstrict-prototypes -Werror-implicit-function-declaration -Warray-bounds -Wdiv-by-zero -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=bounds -Wshadow -D_FORTIFY_SOURCE=0 -fsanitize=undefined -fno-sanitize-recover=all -Wformat=2 -std=gnu++2b -DLOCAL")

#tests
enable_testing()
add_subdirectory(tests)
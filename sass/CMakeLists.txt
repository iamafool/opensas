# CMakeList.txt : CMake project for sass, include source and define
# project specific logic here.
#

include_directories("c:/workspace/c++/spdlog-1.15.0/include")

add_compile_options(/utf-8)

add_library (libsass
"Token.h" "Lexer.h" "Lexer.cpp" "AST.h" "Parser.h" "Parser.cpp" "DataEnvironment.h" "Interpreter.h" "Interpreter.cpp" "Sorter.h"
 "utility.h")

# Add source to this project's executable.
add_executable (sass "sass.cpp" "sass.h" "Repl.h" "Repl.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET libsass PROPERTY CXX_STANDARD 20)
  set_property(TARGET sass PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(sass PRIVATE libsass)

# TODO: Add tests and install targets if needed.

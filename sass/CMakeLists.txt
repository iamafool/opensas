# CMakeList.txt : CMake project for sass, include source and define
# project specific logic here.
#

include_directories("c:/workspace/c++/spdlog-1.15.0/include")

add_compile_options(/utf-8)

# Add source to this project's executable.
add_executable (sass "sass.cpp" "sass.h" "Token.h" "Lexer.h" "Lexer.cpp" "AST.h" "Parser.h" "Parser.cpp" "DataEnvironment.h" "Interpreter.h" "Interpreter.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET sass PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.

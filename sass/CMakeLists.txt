# CMakeList.txt : CMake project for sass, include source and define
# project specific logic here.
#

include_directories("c:/workspace/c++/spdlog-1.15.0/include")
include_directories("c:/workspace/c++/boost_1_86_0/")
include_directories("c:/workspace/c++/sass/sass")
include_directories("c:/workspace/c++/sass/sass/include")

add_compile_options(/utf-8)

add_library (libsass
    "Token.h"
    "Lexer.h"
    "Lexer.cpp"
    "AST.h"
    "Parser.h"
    "Parser.cpp"
    "DataEnvironment.h"
    "Interpreter.h"
    "Interpreter.cpp"
    "Sorter.h"
    "utility.h"
    "sasdoc.h"
    "sasdoc.cpp"
    "Dataset.h"
    "PDV.h"
    "DataEnvironment.cpp"
    readstat/sas/ieee.c
    readstat/sas/readstat_sas.c
    readstat/sas/readstat_sas_rle.c
    readstat/sas/readstat_sas7bcat_read.c
    readstat/sas/readstat_sas7bcat_write.c
    readstat/sas/readstat_sas7bdat_read.c
    readstat/sas/readstat_sas7bdat_write.c
    readstat/sas/readstat_xport.c
    readstat/sas/readstat_xport_parse_format.c
    readstat/sas/readstat_xport_read.c
    readstat/sas/readstat_xport_write.c
    readstat/readstat_bits.c
    readstat/readstat_convert.c
    readstat/readstat_error.c
    readstat/readstat_io_unistd.c
    readstat/readstat_malloc.c
    readstat/readstat_metadata.c
    readstat/readstat_parser.c
    readstat/readstat_value.c
    readstat/readstat_variable.c
    readstat/readstat_writer.c 
 "PDV.cpp" "Library.h" "Library.cpp" "TempUtils.h" "TempUtils.cpp" "StepTimer.h" "StepTimer.cpp")

# Add source to this project's executable.
add_executable (sass "sass.cpp" "sass.h" "Repl.h" "Repl.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET libsass PROPERTY CXX_STANDARD 20)
  set_property(TARGET sass PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(libsass PRIVATE c:/workspace/c++/sass/sass/libiconv.lib)
target_link_libraries(sass PRIVATE libsass)

# TODO: Add tests and install targets if needed.

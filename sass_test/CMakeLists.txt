# Enable testing
enable_testing()

# Add Google Test
include_directories("c:/workspace/c++/spdlog-1.15.0/include")
include_directories("c:/workspace/c++/googletest-main/googletest/include/")
include_directories("c:/workspace/c++/sass/sass/")
link_directories("c:/workspace/c++/googletest-main/build/lib/Debug/")

add_compile_options(/utf-8)

# Add your test executable
add_executable(runTests test_main.cpp   "data_step.cpp" fixture.h)

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET runTests PROPERTY CXX_STANDARD 20)
endif()

# Link libraries
target_link_libraries(runTests libsass gtest)
   
# Add tests
add_test(NAME sass_test COMMAND runTests)

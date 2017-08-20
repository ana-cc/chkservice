add_executable(RunTests run-tests.cpp)
target_link_libraries(RunTests ADDLIB)

add_custom_target(Test COMMAND ./RunTests)

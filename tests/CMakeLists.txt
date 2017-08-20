add_executable(RunTests main-test.cpp unit1-test.cpp)
target_link_libraries(RunTests ${LIBS})

add_custom_target(Test COMMAND ./RunTests)

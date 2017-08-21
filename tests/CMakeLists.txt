add_executable(RunTests main-test.cpp chksystemd-test.cpp)
target_link_libraries(RunTests ${LIBS} CHKSYSTEMD)

add_custom_target(Test COMMAND sudo ./RunTests)

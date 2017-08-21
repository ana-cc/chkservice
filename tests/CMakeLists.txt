add_executable(RunTests main-test.cpp chksystemd-test.cpp chkctl-test.cpp)
target_link_libraries(RunTests ${LIBS} CHKSYSTEMD CHKCTL)

add_custom_target(Test COMMAND sudo ./RunTests)

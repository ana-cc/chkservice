add_executable(RunTests main-test.cpp chksystemd-test.cpp chkctl-test.cpp chkui-test.cpp)
target_link_libraries(RunTests ${LIBS} CHKSYSTEMD CHKCTL CHKUI)

add_custom_target(Test COMMAND sudo ./RunTests)

add_library(CHKSYSTEMD chk-systemd.cpp chk-systemd-utils.cpp)
target_link_libraries(CHKSYSTEMD ${LIBS})

add_library(CHKCTL chk-ctl.cpp)
target_link_libraries(CHKCTL ${LIBS} CHKSYSTEMD)

add_executable(chkservice chkservice.cpp)
target_link_libraries(chkservice ${LIBS} CHKSYSTEMD CHKCTL)


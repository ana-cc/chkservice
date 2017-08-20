project(chkservice)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ($ENV{RELEASE})
  set (CMAKE_CXX_FLAGS "-O2 -Wall")
else()
  set (CMAKE_CXX_FLAGS "-g -Wall")
endif()

if ($ENV{TRAVIS})
  message(STATUS "Travis-CI build.")
else()
  message(STATUS "Local build")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

find_package(PkgConfig REQUIRED)

pkg_check_modules(SYSTEMD libsystemd)

include_directories(${SYSTEMD_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SYSTEMD_LIBRARIES})

pkg_check_modules(NCURSES ncurses)

include_directories(${NCURSES_INCLUDE_DIRS})
set(LIBS ${LIBS} ${NCURSES_LIBRARIES})

# Cmake wrapper does not work properly with tests
# enable_testing()

add_subdirectory(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
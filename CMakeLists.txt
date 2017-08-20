project(chkservice)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ($ENV{DEBUG})
  message(STATUS "DEBUG mode enabled")
  set (CMAKE_CXX_FLAGS "-g -Wall")
else()
  message(STATUS "DEBUG mode disabled")
  set (CMAKE_CXX_FLAGS "-O2")
endif()

if ($ENV{TRAVIS})
  message(STATUS "Travis-CI build.")
else()
  message(STATUS "Local build")
endif()

include_directories("${PROJECT_SOURCE_DIR}/include")

find_package(PkgConfig REQUIRED)

pkg_check_modules(SYSTEMD libsystemd REQUIRED)

include_directories(${SYSTEMD_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SYSTEMD_LIBRARIES})

pkg_check_modules(NCURSES ncurses REQUIRED)

include_directories(${NCURSES_INCLUDE_DIRS})
set(LIBS ${LIBS} ${NCURSES_LIBRARIES})

add_subdirectory(${PROJECT_SOURCE_DIR}/src)

if ($ENV{DEBUG})
  message(STATUS "Build testing support")
  add_subdirectory(${PROJECT_SOURCE_DIR}/tests)
endif()
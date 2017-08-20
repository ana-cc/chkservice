# To build components as shared libraries use SHARED keyword
# add_library(ADDLIB SHARED add.cpp)

#add_library(ADDLIB add.cpp)

add_executable(chkservice chkservice.cpp)
target_link_libraries(chkservice ${LIBS})


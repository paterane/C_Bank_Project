cmake_minimum_required(VERSION 3.0.0)
project(Online_Banking)
# C standard
set(CMAKE_C_STANDARD 11)
# executable output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/executable)

# add source code to make executable
add_executable(bank main.c)
# make library path public
target_include_directories(bank PUBLIC include)
# handling library
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_library(bnkfunc STATIC include/bankHandle.c include/utils.c)
find_library(BNK bnkfunc lib)
target_link_libraries(bank PUBLIC ${BNK})

# target_link_libraries(bank PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/library/libbnkfunc.a)
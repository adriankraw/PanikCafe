cmake_minimum_required(VERSION 3.15)
project(Game01)

set(CMAKE_C_STANDARD 17) 
set(Header ./include/*.h)

add_executable(${PROJECT_NAME} ./src/main.cpp ${HEADER})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE ./include)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/lib/libraylib.a)
endif()

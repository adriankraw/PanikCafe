cmake_minimum_required(VERSION 3.15)
project(Game01)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if (UNIX)
    find_package(X11 REQUIRED)
endif()

set(HEADER ${CMAKE_CURRENT_LIST_DIR}/external/)
set(IMGUI 
    external/imgui/imgui.h
    external/imgui/imgui.cpp
    external/imgui/imgui_draw.cpp
    external/imgui/imgui_tables.cpp
    external/imgui/imgui_widgets.cpp
    external/imgui/imgui_demo.cpp
    external/rlImGui/rlImGui.cpp
    external/rlImGui/rlImGui.h
    external/rlImGui/imgui_impl_raylib.h
)

add_executable(${PROJECT_NAME} ./src/main.cpp ${HEADER} ${IMGUI} )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME} PRIVATE ./external)
target_include_directories(${PROJECT_NAME} PRIVATE ./external/imgui)
target_include_directories(${PROJECT_NAME} PRIVATE ./external/json/single_include/nlohmann)
target_include_directories(${PROJECT_NAME} PRIVATE ./external/rlImGui)
target_include_directories(${PROJECT_NAME} PRIVATE ./external/raylib/src/include)
target_include_directories(${PROJECT_NAME} PRIVATE ./external/raygui/src)

# Checks if OSX and links appropriate frameworks (only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
    target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/external/raylib/src/libraylib.a)
endif()
if (UNIX)
    target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_LIST_DIR}/external/raylib/src/libraylib.a)
    target_link_libraries(${PROJECT_NAME} X11::X11)
endif()

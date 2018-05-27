cmake_minimum_required(VERSION 3.9)

project(database)

find_package(Boost COMPONENTS system REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_library(tablemanager tablemanager.cpp)
add_executable(data main.cpp)

set_target_properties(data tablemanager PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        # COMPILE_DEFINITIONS BOOST_ASIO_ENABLE_HANDLER_TRACKING
        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
        )

target_link_libraries(data
    tablemanager
    ${Boost_LIBRARIES}
)

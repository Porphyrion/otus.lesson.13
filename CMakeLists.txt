cmake_minimum_required(VERSION 3.9)

project(join_server) # VERSION 0.0.$ENV{TRAVIS_BUILD_NUMBER})

find_package(Boost COMPONENTS system REQUIRED)
find_package(Threads)
include_directories(${Boost_INCLUDE_DIR})

add_library(tablemanager tablemanager.cpp)
add_executable(join_server main.cpp)

set_target_properties(join_server tablemanager PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON
        # COMPILE_DEFINITIONS BOOST_ASIO_ENABLE_HANDLER_TRACKING
        COMPILE_OPTIONS "-Wpedantic;-Wall;-Wextra"
        )

target_link_libraries(join_server
    tablemanager
    ${Boost_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS join_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT ivanbasurman88@yandex.ru)

include(CPack)

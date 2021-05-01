cmake_minimum_required(VERSION 3.19)
project(thebotx_cxx)

if (${CMAKE_CURRENT_LIST_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)

find_package(ctre REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)

add_library(ch)
target_include_directories(ch PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
target_sources(ch
PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include/ch/get_server.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/ch/gen_uid.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/ch/ChannelFlags.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/ch/parse_font.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/ch/Room.hpp
    ${CMAKE_CURRENT_LIST_DIR}/include/ch/RoomManager.hpp
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/get_server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/gen_uid.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/parse_font.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/RoomManager.cpp
)
target_link_libraries(ch PRIVATE ctre::ctre OpenSSL::OpenSSL fmt::fmt spdlog::spdlog Boost::Boost)

find_package(spdlog REQUIRED)

add_executable(thebotx_cxx src/main.cpp)
target_link_libraries(thebotx_cxx PRIVATE ch Boost::Boost fmt::fmt spdlog::spdlog)
target_compile_options(thebotx_cxx PRIVATE -fcoroutines)
target_include_directories(thebotx_cxx PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

install(TARGETS thebotx_cxx ch
    ARCHIVE DESTINATION lib/static/
    LIBRARY DESTINATION lib/
    RUNTIME DESTINATION bin/
)

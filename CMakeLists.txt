cmake_minimum_required(VERSION 3.16)
project(thebotx_cxx)

if (${CMAKE_CURRENT_LIST_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake OPTIONAL)

file(DOWNLOAD https://github.com/andreasbuhr/cppcoro/raw/master/cmake/FindCoroutines.cmake ${CMAKE_BINARY_DIR}/FindCoroutines.cmake)

# list(APPEND CMAKE_MODULE_PATH "${googletest_SOURCE_DIR}/cmake")

find_package(ctre REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)


if (MSVC)
    add_compile_options(/bigobj /W4)
else()
    add_compile_options(-Wall -Wextra --pedantic-errors)
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag(-Weverything HAS_WEVERYTHING)
    if(HAS_WEVERYTHING)
        add_compile_options(-Weverything -Wno-c++98-compat -Wno-padded -Wno-exit-time-destructors -Wno-global-constructors)
    endif()
endif()

if (CMAKE_CXX_COMPILE_ID STREQUAL "Clang")
    add_compile_options(-stdlib=libc++)
endif()

find_package(Coroutines COMPONENTS Experimental Final REQUIRED)

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
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/parse_name_color.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/get_anon_id.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/rcmds.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/rcmds/i.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/rcmds/pong.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/rcmds/ok.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/rcmds/v.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/io.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/connect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/Room/disconnect.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/ch/RoomManager.cpp
)
target_link_libraries(ch PRIVATE ctre::ctre OpenSSL::OpenSSL spdlog::spdlog)
target_link_libraries(ch PUBLIC std::coroutines Boost::Boost fmt::fmt)

add_executable(thebotx_cxx)
target_sources(thebotx_cxx PRIVATE
${CMAKE_CURRENT_LIST_DIR}/src/thebotx/main.cpp
)
target_link_libraries(thebotx_cxx PRIVATE ch spdlog::spdlog)
target_include_directories(thebotx_cxx PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

install(TARGETS thebotx_cxx ch
    ARCHIVE DESTINATION lib/static/
    LIBRARY DESTINATION lib/
    RUNTIME DESTINATION bin/
)

cmake_minimum_required(VERSION 3.0...3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_VERSION_MINIMUM 3.1)

project(SteamClient LANGUAGES CXX)

include(CTest)

set(BOOST_COMPONENTS system)

if(BUILD_TESTING)
    list(APPEND BOOST_COMPONENTS unit_test_framework)
endif()

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})

find_package(CURL REQUIRED)

include(FetchContent)
include(cmake/get_cpm.cmake)

# LOGGING
find_package(spdlog REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_library(SteamClient ${SOURCES})

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
CPMAddPackage(
    NAME protobuf
    GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
    GIT_TAG "v33.5"
    OPTIONS
        "protobuf_BUILD_TESTS OFF"
        "protobuf_USE_PACKAGE_ABSL ON"
)

find_package(cryptopp REQUIRED)

target_link_libraries(SteamClient
    PUBLIC
        Boost::system
    PRIVATE
        protobuf::libprotobuf
        spdlog::spdlog_header_only
        CURL::libcurl
        cryptopp::cryptopp
)

target_include_directories(SteamClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(WIN32)
    target_link_libraries(SteamClient PRIVATE ws2_32)
    target_compile_definitions(SteamClient PRIVATE _WIN32_WINNT=0x0601)
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
    cmake/SteamClientConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/SteamClientConfig.cmake
    INSTALL_DESTINATION lib/cmake/SteamClient
)

install(TARGETS SteamClient
    EXPORT SteamClientTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

install(EXPORT SteamClientTargets
    FILE SteamClientTargets.cmake
    NAMESPACE SteamClient::
    DESTINATION lib/cmake/SteamClient
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SteamClientConfig.cmake
    DESTINATION lib/cmake/SteamClient
)
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

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

add_library(SteamClient ${SOURCES})

option(STEAMCLIENT_ENABLE_LOGGING "Enable spdlog logging" ON)

if(STEAMCLIENT_ENABLE_LOGGING)
    find_package(spdlog QUIET)
    if(spdlog_FOUND)
        target_link_libraries(SteamClient PRIVATE spdlog::spdlog)
        target_compile_definitions(SteamClient PRIVATE STEAMCLIENT_USE_SPDLOG)
    endif()
endif()

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

find_package(Protobuf REQUIRED)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(CRYPTOPP libcrypto++)
endif()

find_library(CRYPTOPP_LIB cryptopp)

target_link_libraries(SteamClient
    PUBLIC
        Boost::system
    PRIVATE
        protobuf::libprotobuf
        CURL::libcurl
        ${CRYPTOPP_LIB}
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
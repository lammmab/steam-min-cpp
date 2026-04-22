cmake_minimum_required(VERSION 3.21)

project(SteamClient VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_EXAMPLES "Build SteamClient examples" OFF)
option(STEAMCLIENT_ENABLE_LOGGING "Enable spdlog logging" OFF)

## == MODULES == ##
include(cmake/dependencies.cmake)
include(cmake/protobuf.cmake)

## == SOURCES == ##
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

add_library(SteamClient 
    ${SOURCES}
    ${GENERATED_PROTO_SRCS}
)

## == OPTIONAL LOGGING == ##
include(cmake/logging.cmake)

target_link_libraries(SteamClient
    PUBLIC
        Boost::system
        protobuf::libprotobuf
        ${CRYPTOPP_LIB}
    PRIVATE
        absl::check
        absl::log
        CURL::libcurl
)

## == INCLUDES == ##
target_include_directories(SteamClient
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROTO_OUTPUT_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

## = WINDOWS NETWORKING == ##
if(WIN32)
    target_link_libraries(SteamClient PRIVATE ws2_32)
    target_compile_definitions(SteamClient PRIVATE _WIN32_WINNT=0x0601)
endif()

## == TESTING == ##
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

## == EXAMPLES == ##
if(BUILD_EXAMPLES) 
    add_subdirectory(examples)
endif()

## == PACKAGING / Versioning == ##
include(CMakePackageConfigHelpers)

# VERSIONING

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/include/steamclient/version.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/generated/steamclient/version.hpp
)

target_include_directories(SteamClient PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)

set_target_properties(SteamClient PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/SteamClientConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/SteamClientConfigVersion.cmake
    DESTINATION lib/cmake/SteamClient
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/generated/steamclient/version.hpp
    DESTINATION include/steamclient
)

# PACKAGING

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

add_library(SteamClient::SteamClient ALIAS SteamClient)
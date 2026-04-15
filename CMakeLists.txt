cmake_minimum_required(VERSION 3.21)

project(SteamClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(BUILD_EXAMPLES "Build SteamClient examples" ON)

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
    PRIVATE
        protobuf::libprotobuf
        absl::check
        absl::log
        CURL::libcurl
        ${CRYPTOPP_LIB}
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

## == PACKAGING == ##
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
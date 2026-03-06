cmake_minimum_required(VERSION 3.0...3.5)
project(SteamClient LANGUAGES CXX)

find_package(CURL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_POLICY_VERSION_MINIMUM 3.1)

include(FetchContent)
include(cmake/get_cpm.cmake)
FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-27-0
)
FetchContent_MakeAvailable(asio)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

file(GLOB_RECURSE SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
)

add_executable(SteamClient ${SOURCES})

target_include_directories(SteamClient PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${asio_SOURCE_DIR}/asio/include
)

set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
CPMAddPackage(
    NAME protobuf
    GIT_REPOSITORY "https://github.com/protocolbuffers/protobuf.git"
    GIT_TAG "v33.5"
    OPTIONS
        "protobuf_BUILD_TESTS OFF"
        "protobuf_USE_PACKAGE_ABSL ON"
)

CPMAddPackage(
	NAME          	cryptopp-cmake
	GIT_REPOSITORY	"https://github.com/abdes/cryptopp-cmake.git"
	GIT_TAG       	"CRYPTOPP_8_9_0"
	OPTIONS       	"CRYPTOPP_BUILD_TESTING OFF" "CRYPTOPP_INSTALL OFF"
)

target_link_libraries(SteamClient PRIVATE protobuf::libprotobuf)
target_include_directories(SteamClient PRIVATE ${protobuf_SOURCE_DIR}/src)

target_link_libraries(SteamClient PRIVATE 
    spdlog::spdlog_header_only
    CURL::libcurl
    cryptopp::cryptopp
    #OpenSSL::SSL
    #OpenSSL::Crypto
)

if(WIN32)
    target_link_libraries(SteamClient PRIVATE ws2_32)
    target_compile_definitions(SteamClient PRIVATE _WIN32_WINNT=0x0601)
endif()
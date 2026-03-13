set(BOOST_COMPONENTS system)

if(BUILD_TESTING)
    list(APPEND BOOST_COMPONENTS unit_test_framework)
endif()

find_package(Boost REQUIRED COMPONENTS ${BOOST_COMPONENTS})
find_package(CURL REQUIRED)
find_package(Protobuf REQUIRED)

find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(CRYPTOPP libcrypto++)
endif()

find_library(CRYPTOPP_LIB cryptopp)
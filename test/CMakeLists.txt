add_executable(unit_tests
    test_main.cpp
    test_stream.cpp
    test_vdf.cpp
)

target_include_directories(unit_tests PRIVATE
    ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(unit_tests
    PRIVATE
    SteamClient
    Boost::unit_test_framework
)

add_test(NAME unit_tests COMMAND unit_tests)
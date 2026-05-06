set(EXAMPLES
    anonymous_login
    get_info
)

foreach(EXAMPLE_NAME ${EXAMPLES})
    add_executable(${EXAMPLE_NAME} ${EXAMPLE_NAME}.cpp)

    target_link_libraries(${EXAMPLE_NAME}
        PRIVATE
            -Wl,--whole-archive
            SteamClient
            -Wl,--no-whole-archive
    )
endforeach()
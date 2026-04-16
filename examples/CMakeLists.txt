file(GLOB EXAMPLE_SOURCES CONFIGURE_DEPENDS *.cpp)

foreach(EXAMPLE_SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)

    add_executable(${EXAMPLE_NAME} ${EXAMPLE_SOURCE})

    target_link_libraries(${EXAMPLE_NAME}
        PRIVATE
            -Wl,--whole-archive
            SteamClient
            -Wl,--no-whole-archive
    )
endforeach()
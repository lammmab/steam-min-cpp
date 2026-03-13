option(STEAMCLIENT_ENABLE_LOGGING "Enable spdlog logging" ON)

if(STEAMCLIENT_ENABLE_LOGGING)
    find_package(spdlog QUIET)

    if(spdlog_FOUND)
        target_link_libraries(SteamClient PRIVATE spdlog::spdlog)
        target_compile_definitions(SteamClient PRIVATE STEAMCLIENT_USE_SPDLOG)
    endif()
endif()
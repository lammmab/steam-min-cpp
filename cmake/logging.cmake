if(STEAMCLIENT_ENABLE_LOGGING)
    find_package(spdlog QUIET)

    if(spdlog_FOUND)
        target_link_libraries(SteamClient PUBLIC spdlog::spdlog)
        target_compile_definitions(SteamClient PRIVATE STEAMCLIENT_USE_SPDLOG)
    endif()
endif()
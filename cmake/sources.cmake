set(STEAM_SOURCES
    ## == CLIENT == ##
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/web/cmfetcher.cpp

    ## == UTILS == ##
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/gzip/gzip_helpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/vdf/vdf_bin.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils/vdf/vdf_text.cpp

    ## == EVENTS == ##
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/events/dispatcher.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/events/handlers/encryption_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/events/handlers/logon_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/events/handlers/multi_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/events/handlers/product_info_handler.cpp
    
    ## == COMMANDS == ##
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/commands/handlers/anon_login_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/commands/handlers/heartbeat_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/commands/handlers/logoff_handler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/routing/commands/handlers/product_info_handler.cpp
    
    ## == NETWORK == ##
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network/cmclient.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/network/connection/tcp.cpp
    
    ## == CRYPTO == ##
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/crypto.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/crypto/helpers.cpp
)
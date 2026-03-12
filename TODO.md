# To Do

- implement get_product_info

- forward declare where possible / clean up includes | Partial
- clean up message headers, use an optimized Stream implementation, fix SteamUtils.hpp
- uniform style, preferrably complying with C++ guidelines
- optimize where possible (fixed-size arrays instead of vectors where possible, etc.)
- ensure extensibility and clarity for future contributers
- implement UDPConnection and fix websocketconnection | Not planned ; another contributor can fix up the WebsocketConnection and create UDPConnection

# Done

- CMClient is an un-concern seperated monolith of a file. ✅
- handle protobuf msgs and multi msgs ✅
- Start using namespaces to not pollute global ✅
- Create class for cryptography ✅
- implement anonymous_login ✅
- is_protobuf_msg is wrong past inital encryption phase ✅
- create IConnection and allow users to specify TCP vs UDP vs websockets ✅

# Nice for the future

- automatic protobuf <-> message registry
- packet tracing/debug logging
- reconnect logic
- heartbeat / keepalive handling
- CMFetcher retries / seperate CM server picking and raw connection layer
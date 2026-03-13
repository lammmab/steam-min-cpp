# To Do

- Create examples that can be optionally be built with CMake
- Add a version into the cmake for widespread support
- Add documentation (readthedocs.io???)
- Implement more, in-depth unit tests for the public-facing API
- Test on different OS (windows / linux)

- reduce per-command boilerplate by automatically adding the sessionid and steamid upon protomsg creation (helper?)
- more events for errors (MsgParseFailureEvent, ConnectionFailureEvent, etc.)

- optimize where possible (fixed-size arrays instead of vectors where possible, etc.)
- implement UDPConnection and fix websocketconnection | Not planned ; another contributor can fix up the WebsocketConnection and create UDPConnection

# Done

- implement get_product_info ✅
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
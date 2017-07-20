# Part of Beast

GroupSources(include/beast beast)

GroupSources(example/websocket-client "/")

add_executable (websocket-client
    ${BOOST_BEAST_INCLUDES}
    ${EXTRAS_INCLUDES}
    websocket_client.cpp
)

target_link_libraries(websocket-client Beast)

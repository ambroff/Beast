# Part of Beast

GroupSources(include/beast beast)

GroupSources(example/websocket-client-ssl "/")

add_executable (websocket-client-ssl
    ${BOOST_BEAST_INCLUDES}
    websocket_client_ssl.cpp
)

target_link_libraries(websocket-client-ssl
    Beast
    ${OPENSSL_LIBRARIES}
    )

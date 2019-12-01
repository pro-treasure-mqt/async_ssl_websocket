set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

aux_source_directory(./ SRC_LIST)

add_library(async_ssl_ws ${SRC_LIST})
target_link_libraries(async_ssl_ws pthread)
target_link_libraries(async_ssl_ws boost_thread)
target_link_libraries(async_ssl_ws boost_system)
target_link_libraries(async_ssl_ws ssl)
target_link_libraries(async_ssl_ws crypto)


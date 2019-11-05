add_library( ft8CodecCommon constants.cpp text.cpp )
target_include_directories( ft8CodecCommon PUBLIC ${CMAKE_CURRENT_LIST_DIR} )

add_library( ft8Encode encode.cpp constants.cpp pack.cpp )
target_include_directories( ft8Encode PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries( ft8Encode PRIVATE ft8CodecCommon )

add_library( ft8Decode decode.cpp constants.cpp unpack.cpp ldpc.cpp encode.cpp )
target_include_directories( ft8Decode PUBLIC ${CMAKE_CURRENT_LIST_DIR} )
target_link_libraries( ft8Decode PRIVATE ft8CodecCommon )

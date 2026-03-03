set(RTP_NET_TEST_SOURCES

)

if(NOT RTP_NET_TEST_SOURCES)
    message(STATUS "No unit tests found in ${CMAKE_CURRENT_SOURCE_DIR}")
    return()
endif()

foreach(TEST_SOURCE IN LISTS RTP_NET_TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)
    set(TEST_TARGET "rtp_net_test_${TEST_NAME}")

    add_executable(${TEST_TARGET} "${TEST_SOURCE}")
    target_link_libraries(${TEST_TARGET} PRIVATE rtp-net)
    target_compile_features(${TEST_TARGET} PRIVATE cxx_std_23)

    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endforeach()

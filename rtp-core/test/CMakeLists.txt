# Keep this list explicit: when you add/remove a test file, update it here.
set(RTP_CORE_TEST_SOURCES
    helper_test.cpp
    random_test.cpp
    rtp_packet_payload_test.cpp
    rtp_packet_setters_test.cpp
)

if(NOT RTP_CORE_TEST_SOURCES)
    message(STATUS "No unit tests found in ${CMAKE_CURRENT_SOURCE_DIR}")
    return()
endif()

foreach(TEST_SOURCE IN LISTS RTP_CORE_TEST_SOURCES)
    get_filename_component(TEST_NAME "${TEST_SOURCE}" NAME_WE)
    set(TEST_TARGET "rtp_core_test_${TEST_NAME}")

    add_executable(${TEST_TARGET} "${TEST_SOURCE}")
    target_link_libraries(${TEST_TARGET} PRIVATE rtp-core)
    target_include_directories(${TEST_TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/rtp-core/private)
    target_compile_features(${TEST_TARGET} PRIVATE cxx_std_23)

    add_test(NAME ${TEST_TARGET} COMMAND ${TEST_TARGET})
endforeach()

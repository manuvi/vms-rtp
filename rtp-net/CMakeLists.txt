set(VMS_RTP_LIBRARY_TYPE STATIC)
if(VMS_RTP_BUILD_SHARED)
    set(VMS_RTP_LIBRARY_TYPE SHARED)
endif()

set(RTP_NET_TARGET_NAME rtp-net)

# Keep this list explicit: when you add/remove a source file, update it here.
set(RTP_NET_SOURCES
    src/rtp-net/RtpNetBridge.cpp
)

add_library(${RTP_NET_TARGET_NAME} ${VMS_RTP_LIBRARY_TYPE} ${RTP_NET_SOURCES})

target_compile_features(${RTP_NET_TARGET_NAME} PUBLIC cxx_std_20)

target_include_directories(
    ${RTP_NET_TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(${RTP_NET_TARGET_NAME} PUBLIC rtp-core)

set_target_properties(${RTP_NET_TARGET_NAME} PROPERTIES OUTPUT_NAME "vms-rtp-net")

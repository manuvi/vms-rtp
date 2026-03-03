set(VMS_RTP_LIBRARY_TYPE STATIC)
if(VMS_RTP_BUILD_SHARED)
    set(VMS_RTP_LIBRARY_TYPE SHARED)
endif()

set(VMS_RTP_TARGET_NAME vms-rtp)

# Keep this list explicit: when you add/remove a source file, update it here.
set(VMS_RTP_SOURCES
    vms-rtp/RtpPacket.cpp
)

add_library(${VMS_RTP_TARGET_NAME} ${VMS_RTP_LIBRARY_TYPE} ${VMS_RTP_SOURCES})

target_compile_features(${VMS_RTP_TARGET_NAME} PUBLIC cxx_std_20)

target_include_directories(
    ${VMS_RTP_TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../private>
)

set_target_properties(${VMS_RTP_TARGET_NAME} PROPERTIES OUTPUT_NAME "vms-rtp")

set(VMS_RTP_LIBRARY_TYPE STATIC)
if(VMS_RTP_BUILD_SHARED)
    set(VMS_RTP_LIBRARY_TYPE SHARED)
endif()

set(RTP_CORE_TARGET_NAME rtp-core)

# Keep this list explicit: when you add/remove a source file, update it here.
set(RTP_CORE_SOURCES
    src/rtp-core/RtpPacket.cpp
)

add_library(${RTP_CORE_TARGET_NAME} ${VMS_RTP_LIBRARY_TYPE} ${RTP_CORE_SOURCES})

target_compile_features(${RTP_CORE_TARGET_NAME} PUBLIC cxx_std_20)

target_include_directories(
    ${RTP_CORE_TARGET_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/private>
)

set_target_properties(${RTP_CORE_TARGET_NAME} PROPERTIES OUTPUT_NAME "vms-rtp-core")

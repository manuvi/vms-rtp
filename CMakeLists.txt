cmake_minimum_required(VERSION 3.16)

project(vms_rtp VERSION 0.1.0 LANGUAGES CXX)

option(VMS_RTP_BUILD_SHARED "Build vms-rtp as a shared library" ON)
option(VMS_RTP_ENABLE_COVERAGE "Enable code coverage instrumentation and report target" OFF)

if(VMS_RTP_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O0 -g --coverage)
        add_link_options(--coverage)
    else()
        message(WARNING "Coverage is enabled, but only GNU/Clang toolchains are supported.")
    endif()
endif()

add_subdirectory(src)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

if(VMS_RTP_ENABLE_COVERAGE)
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
            COMMAND ${LCOV_EXECUTABLE} --directory ${CMAKE_BINARY_DIR} --zerocounters
            COMMAND ${LCOV_EXECUTABLE} --capture --initial --directory ${CMAKE_BINARY_DIR} --output-file coverage_base.info
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --test-dir ${CMAKE_BINARY_DIR}
            COMMAND ${LCOV_EXECUTABLE} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage_test.info
            COMMAND ${LCOV_EXECUTABLE} --add-tracefile coverage_base.info --add-tracefile coverage_test.info --output-file coverage_total.info
            COMMAND ${LCOV_EXECUTABLE} --remove coverage_total.info '/usr/*' --output-file coverage.info
            COMMAND ${GENHTML_EXECUTABLE} coverage.info --output-directory coverage-html
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            USES_TERMINAL
            COMMENT "Build, run tests, and generate coverage report in coverage-html/"
        )
    else()
        message(WARNING "Coverage report target not created: install lcov and genhtml.")
    endif()
endif()

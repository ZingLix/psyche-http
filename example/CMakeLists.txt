option(PSYCHE_HTTP_BUILD_ALL_EXAMPLES "Build psyche http examples" OFF)
IF(PSYCHE_HTTP_BUILD_ALL_EXAMPLES)
add_subdirectory(simple_server)
endif()
if(PSYCHE_HTTP_BUILD_EXAMPLE)
add_subdirectory(${PSYCHE_HTTP_BUILD_EXAMPLE})
endif()

# Check if examples are enabled
if(NOT OPTION_BUILD_APPS)
    return()
endif()

# Applications
add_subdirectory(moria-timelapse)
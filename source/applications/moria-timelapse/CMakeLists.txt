
# 
# External dependencies
# 

# find_package(THIRDPARTY REQUIRED)

find_package(OpenCV REQUIRED)
find_package(Boost 1.66 REQUIRED program_options)

# 
# Executable name and optionsw
# 

# Target name
set(target moria)

# Exit here if required dependencies are not met
message(STATUS "Moria ${target}")

# 
# Sources
# 

set(sources
    util.cpp
    FPSCounter.cpp
    IntervalTimer.cpp
    CameraManager.cpp
    moria_options_boost.cpp
    moria_options.cpp
    moria.cpp
    main.cpp
)

# 
# Create executable
# 

# Build executable
add_executable(${target}
    MACOSX_BUNDLE
    ${sources}
)

# Create namespaced alias
add_executable(${META_PROJECT_NAME}::${target} ALIAS ${target})

# 
# Project options
# 

set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    FOLDER "${IDE_FOLDER}"
)


# 
# Include directories
# 

target_include_directories(${target}
    PRIVATE
    /usr/local/include/opencv4
    ${DEFAULT_INCLUDE_DIRECTORIES}
    ${PROJECT_BINARY_DIR}/source/include
)


# 
# Libraries
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LIBRARIES}
    ${META_PROJECT_NAME}::baselib
    ${OpenCV_LIBS}
    ${Boost_LIBRARIES}
)


# 
# Compile definitions
# 

target_compile_definitions(${target}
    PRIVATE
    ${DEFAULT_COMPILE_DEFINITIONS}
)


# 
# Compile options
# 

target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

#
# Features
#

target_compile_features(${target}
    PRIVATE
    cxx_lambdas
    cxx_generic_lambdas
)


# 
# Linker options
# 

target_link_libraries(${target}
    PRIVATE
    ${DEFAULT_LINKER_OPTIONS}
)


#
# Target Health
#

perform_health_checks(
    ${target}
    ${sources}
)

generate_coverage_report(${target})


# 
# Deployment
# 

# Executable
install(TARGETS ${target}
    RUNTIME DESTINATION ${INSTALL_BIN} COMPONENT examples
    BUNDLE  DESTINATION ${INSTALL_BIN} COMPONENT examples
)

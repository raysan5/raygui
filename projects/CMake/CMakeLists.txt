cmake_minimum_required(VERSION 3.11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(raygui C)

# Config options
option(BUILD_RAYGUI_EXAMPLES "Build the examples." OFF)

# Force building examples if building in the root as standalone.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(BUILD_RAYGUI_EXAMPLES TRUE)
endif()

# Directory Variables
set(RAYGUI_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../..)
set(RAYGUI_SRC ${RAYGUI_ROOT}/src)
set(RAYGUI_EXAMPLES ${RAYGUI_ROOT}/examples)

# raygui
add_library(raygui INTERFACE)
file(GLOB sources ${RAYGUI_SRC}/*.h)
set(RAYGUI_HEADERS ${sources})
install(FILES
    ${RAYGUI_HEADERS} DESTINATION include
)
target_include_directories(raygui INTERFACE ${RAYGUI_SRC})

# Examples
if(${BUILD_RAYGUI_EXAMPLES})
    find_package(Raylib)

    # Get the sources together
    set(example_dirs
        controls_test_suite
        custom_file_dialog
        custom_sliders
        image_exporter
        image_importer_raw
        portable_window
        property_list
        scroll_panel
        style_selector
    )

    set(example_sources)
    set(example_resources)

    foreach(example_dir ${example_dirs})
        # Get the .c files
        file(GLOB sources ${RAYGUI_EXAMPLES}/${example_dir}/*.c)
        list(APPEND example_sources ${sources})

        # Any any resources
        file(GLOB resources ${RAYGUI_EXAMPLES}/${example_dir}/resources/*)
        list(APPEND example_resources ${resources})
    endforeach()

    # Do each example
    foreach(example_source ${example_sources})
        # Create the basename for the example
        get_filename_component(example_name ${example_source} NAME)
        string(REPLACE ".c" "${OUTPUT_EXT}" example_name ${example_name})

        # Setup the example
        add_executable(${example_name} ${example_source})

        target_link_libraries(${example_name} PUBLIC raylib raygui)

        string(REGEX MATCH ".*/.*/" resources_dir ${example_source})
        string(APPEND resources_dir "resources")
    endforeach()

    # Copy all of the resource files to the destination
    file(COPY ${example_resources} DESTINATION "resources/")
endif()
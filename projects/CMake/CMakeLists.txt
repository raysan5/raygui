cmake_minimum_required(VERSION 3.11)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

project(raygui C)

# Config options
option(BUILD_RAYGUI_EXAMPLES "Build the examples." ON)

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
        custom_file_dialog
        image_raw_importer
        portable_window
        scroll_panel
        text_box_selection
        controls_test_suite
        image_exporter
        property_list
        text_editor
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
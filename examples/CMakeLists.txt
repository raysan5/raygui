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
	standalone
	text_editor
)

set(example_sources)
set(example_resources)

foreach(example_dir ${example_dirs})
  # Get the .c files
  file(GLOB sources ${example_dir}/*.c)
  list(APPEND example_sources ${sources})

  # Any any resources
  file(GLOB resources ${example_dir}/resources/*)
  list(APPEND example_resources ${resources})
endforeach()

# Do each example
foreach(example_source ${example_sources})
  # Create the basename for the example
  get_filename_component(example_name ${example_source} NAME)
  string(REPLACE ".c" "${OUTPUT_EXT}" example_name ${example_name})

  # Setup the example
  add_executable(${example_name} ${example_source})

  target_link_libraries(${example_name} ${raylib_LIBRARIES} raygui)

  string(REGEX MATCH ".*/.*/" resources_dir ${example_source})
  string(APPEND resources_dir "resources")
endforeach()

# Copy all of the resource files to the destination
file(COPY ${example_resources} DESTINATION "resources/")

add_library(raygui INTERFACE)

file(GLOB sources *.h)
set(RAYGUI_HEADERS ${sources})

install(FILES
	${RAYGUI_HEADERS} DESTINATION include
)

target_include_directories(raygui INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/)

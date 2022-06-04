set(TARGET Hello)

add_executable(
	${TARGET}
	Main.cpp
)

target_include_directories(
	${TARGET}
	PUBLIC ${OctaneGUI_INCLUDE}
	PUBLIC ${FRONTEND_INCLUDE}
)

target_link_libraries(
	${TARGET}
	Frontend
	OctaneGUI
)

set_target_properties(
	${TARGET}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

add_custom_command(
	TARGET ${TARGET}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different ${APP_FONTS} ${BIN_DIR}
)

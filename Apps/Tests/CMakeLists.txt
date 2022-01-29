set(TARGET Tests)

add_executable(
	${TARGET}
	Json.cpp
	Main.cpp
	TestSuite.cpp
)

target_include_directories(
	${TARGET}
	PUBLIC ${OCTANEUI_INCLUDE}
)

target_link_libraries(
	${TARGET}
	OctaneUI
)

set_target_properties(
	${TARGET}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
)

add_custom_command(
	TARGET ${TARGET}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy ${APP_FONT} ${BIN_DIR}
)

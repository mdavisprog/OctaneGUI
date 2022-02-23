set(TARGET Overview)

add_executable(
	${TARGET}
	Main.cpp
)

target_include_directories(
	${TARGET}
	PUBLIC ${OCTANEUI_INCLUDE}
	PUBLIC ${FRONTEND_INCLUDE}
)

target_link_directories(
	${TARGET}
	PUBLIC ${FRONTEND_LIBS_PATH}
)

target_link_libraries(
	${TARGET}
	${FRONTEND_LIBS}
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
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${FRONTEND_BIN_PATH} ${BIN_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy ${APP_FONT} ${APP_INFO_IMAGE} ${APP_THUMBS_UP_IMAGE} ${BIN_DIR}
)

add_custom_command(
	OUTPUT ${BIN_DIR}/Overview.json
	COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Overview.json ${BIN_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Overview.json
)

add_custom_target(
	Overview_Json ALL
	DEPENDS ${BIN_DIR}/Overview.json
)

add_dependencies(
	${TARGET}
	Overview_Json
)

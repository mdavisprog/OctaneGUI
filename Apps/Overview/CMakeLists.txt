set(TARGET Overview)

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

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BIN_DIR})
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR})

set_target_properties(
	${TARGET}
	PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
)

add_custom_command(
	TARGET ${TARGET}
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${APP_RESOURCES} ${BIN_DIR}
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

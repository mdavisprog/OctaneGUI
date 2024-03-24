set(TARGET Blank)

add_executable(
    ${TARGET}
    Main.cpp
)

target_include_directories(
    ${TARGET}
    PUBLIC ${OctaneGUI_INCLUDE}
    PUBLIC ${FRONTEND_INCLUDE}
)

set(DEBUG_LIB
    Frontend
    OctaneGUI
    sfml-system-d
    sfml-graphics-d
    sfml-window-d
)

set(RELEASE_LIB
    Frontend
    OctaneGUI
    sfml-system
    sfml-graphics
    sfml-window
)

target_link_libraries(
    ${TARGET}
    $<$<CONFIG:Debug>:${DEBUG_LIB}>;$<$<CONFIG:Release>:${RELEASE_LIB}>
)

set_target_properties(
    ${TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
)

add_custom_command(
    OUTPUT ${BIN_DIR}/Blank.json
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Blank.json ${BIN_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Blank.json
)

add_custom_target(
    Blank_Json ALL
    DEPENDS ${BIN_DIR}/Blank.json
)

add_dependencies(
    ${TARGET}
    Blank_Json
)

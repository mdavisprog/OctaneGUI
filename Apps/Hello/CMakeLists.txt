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

set(TARGET CustomLoop)

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
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
)

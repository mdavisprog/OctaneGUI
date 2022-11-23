set(TARGET Tests)

add_executable(
    ${TARGET}
    Button.cpp
    CheckBox.cpp
    ComboBox.cpp
    Container.cpp
    FlyString.cpp
    Json.cpp
    ListBox.cpp
    Main.cpp
    MenuBar.cpp
    RadioButton.cpp
    Rect.cpp
    Scrollable.cpp
    Splitter.cpp
    Table.cpp
    TestSuite.cpp
    Text.cpp
    TextInput.cpp
    Utility.cpp
    Variant.cpp
)

target_include_directories(
    ${TARGET}
    PUBLIC ${OctaneGUI_INCLUDE}
)

target_link_libraries(
    ${TARGET}
    OctaneGUI
)

set_target_properties(
    ${TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BIN_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BIN_DIR}
)

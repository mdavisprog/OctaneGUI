set(TARGET OctaneGUI)

set(DEFINES "")

set(SOURCE
    Controls/Syntax/Highlighter.cpp
    Controls/Syntax/Rules.cpp
    Controls/BoxContainer.cpp
    Controls/Button.cpp
    Controls/Canvas.cpp
    Controls/CheckBox.cpp
    Controls/ComboBox.cpp
    Controls/Container.cpp
    Controls/Control.cpp
    Controls/ControlList.cpp
    Controls/CustomControl.cpp
    Controls/GroupBox.cpp
    Controls/HorizontalContainer.cpp
    Controls/Image.cpp
    Controls/ImageButton.cpp
    Controls/ListBox.cpp
    Controls/MarginContainer.cpp
    Controls/Menu.cpp
    Controls/MenuBar.cpp
    Controls/MenuItem.cpp
    Controls/Panel.cpp
    Controls/RadioButton.cpp
    Controls/ScrollableContainer.cpp
    Controls/ScrollableViewControl.cpp
    Controls/ScrollBar.cpp
    Controls/Separator.cpp
    Controls/Spinner.cpp
    Controls/Splitter.cpp
    Controls/Table.cpp
    Controls/Text.cpp
    Controls/TextButton.cpp
    Controls/TextEditor.cpp
    Controls/TextInput.cpp
    Controls/TextSelectable.cpp
    Controls/Tree.cpp
    Controls/VerticalContainer.cpp
    Controls/WindowContainer.cpp
    Dialogs/FileDialog.cpp
    Dialogs/MessageBox.cpp
    Alignment.cpp
    Application.cpp
    Assert.cpp
    Class.cpp
    Clock.cpp
    Color.cpp
    CommandLine.cpp
    DrawCommand.cpp
    Event.cpp
    FileSystem.cpp
    FlyString.cpp
    Font.cpp
    Icons.cpp
    Json.cpp
    LanguageServer.cpp
    Network.cpp
    Orientation.cpp
    Paint.cpp
    Popup.cpp
    Rect.cpp
    Socket.cpp
    String.cpp
    Texture.cpp
    TextureCache.cpp
    Theme.cpp
    ThemeProperties.cpp
    Timer.cpp
    Variant.cpp
    Vector2.cpp
    Vertex.cpp
    VertexBuffer.cpp
    Window.cpp
)

if(WITH_LSTALK)
    message("Compiling LSTalk library...")
    set(SOURCE ${SOURCE} External/lstalk/lstalk.c)
    set(DEFINES ${DEFINES} WITH_LSTALK=1)
endif()

if(TOOLS)
    set(SOURCE
        ${SOURCE}
        Tools/CommandPalette.cpp
        Tools/Inspector.cpp
        Tools/Mouse.cpp
        Tools/Profiler.cpp
        Tools/ProfileViewer.cpp
        Tools/Properties.cpp
        Tools/TextureViewer.cpp
        Tools/Tools.cpp
    )
endif()

add_library(
    ${TARGET}
    STATIC
    ${SOURCE}
)

target_compile_definitions(
    ${TARGET}
    PUBLIC ${DEFINES}
)

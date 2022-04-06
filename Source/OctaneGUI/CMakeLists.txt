set(TARGET OctaneGUI)

set(SOURCE
	Controls/BoxContainer.cpp
	Controls/Button.cpp
	Controls/CheckBox.cpp
	Controls/ComboBox.cpp
	Controls/Container.cpp
	Controls/Control.cpp
	Controls/ControlList.cpp
	Controls/HorizontalContainer.cpp
	Controls/Image.cpp
	Controls/ImageButton.cpp
	Controls/ListBox.cpp
	Controls/MarginContainer.cpp
	Controls/Menu.cpp
	Controls/MenuBar.cpp
	Controls/MenuItem.cpp
	Controls/Panel.cpp
	Controls/ScrollableContainer.cpp
	Controls/ScrollableViewControl.cpp
	Controls/ScrollBar.cpp
	Controls/Separator.cpp
	Controls/Text.cpp
	Controls/TextButton.cpp
	Controls/TextInput.cpp
	Controls/TextSelectable.cpp
	Controls/VerticalContainer.cpp
	Application.cpp
	Class.cpp
	Clock.cpp
	Color.cpp
	DrawCommand.cpp
	Event.cpp
	Font.cpp
	Icons.cpp
	Json.cpp
	Paint.cpp
	Popup.cpp
	Rect.cpp
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

if(TOOLS)
	message("Building Tools...")
	add_compile_definitions(TOOLS=1)
	set(SOURCE
		${SOURCE}
		Tools/CommandPalette.cpp
	)
endif()

add_library(
	${TARGET}
	STATIC
	${SOURCE}
)

target_include_directories(
	${TARGET}
	PUBLIC ${STB_INCLUDE}
)
set(TARGET OctaneUI)

set(SOURCE
	Controls/Button.cpp
	Controls/Checkbox.cpp
	Controls/Container.cpp
	Controls/Control.cpp
	Controls/ControlList.cpp
	Controls/HorizontalContainer.cpp
	Controls/Image.cpp
	Controls/Menu.cpp
	Controls/MenuBar.cpp
	Controls/MenuItem.cpp
	Controls/Panel.cpp
	Controls/Separator.cpp
	Controls/Text.cpp
	Controls/TextInput.cpp
	Controls/TextSelectable.cpp
	Controls/VerticalContainer.cpp
	Application.cpp
	Class.cpp
	Color.cpp
	Event.cpp
	Font.cpp
	Icons.cpp
	Json.cpp
	Paint.cpp
	Popup.cpp
	Rect.cpp
	Texture.cpp
	Theme.cpp
	Variant.cpp
	Vector2.cpp
	Vertex.cpp
	VertexBuffer.cpp
	Window.cpp
)

add_library(
	${TARGET}
	STATIC
	${SOURCE}
)

target_include_directories(
	${TARGET}
	PUBLIC ${STB_INCLUDE}
)

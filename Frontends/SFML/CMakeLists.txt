set(TARGET Frontend)

set(
	SOURCE
	Interface.cpp
)

add_library(
	${TARGET}
	STATIC
	${SOURCE}
)

target_include_directories(
	${TARGET}
	PUBLIC ${OctaneGUI_INCLUDE}
)

target_link_libraries(
	${TARGET}
	sfml-graphics
)

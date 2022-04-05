set(TARGET SFML_Frontend)

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
	PUBLIC ${SFML_INCLUDE}
	PUBLIC ${STB_INCLUDE}
	PUBLIC ${OctaneGUI_INCLUDE}
)

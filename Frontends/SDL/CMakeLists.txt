set(TARGET SDL_Frontend)

set(
	SOURCE
	Interface.mm
)

add_library(
	${TARGET}
	STATIC
	${SOURCE}
)

target_include_directories(
	${TARGET}
	PUBLIC ${SDL_INCLUDE}
	PUBLIC ${STB_INCLUDE}
	PUBLIC ${OCTANEUI_INCLUDE}
)

find_library(COCOA Cocoa)
find_library(METAL Metal)

target_link_libraries(
	${TARGET}
	${COCOA}
	${METAL}
)

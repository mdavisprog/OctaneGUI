set(TARGET Frontend)

# Default settings for each platform.
if(APPLE)
	set(WINDOWING SDL)
	set(RENDERING METAL)
else()
	set(WINDOWING SFML)
	set(RENDERING SFML)
endif()

message("Desired windowing library: ${WINDOWING}")
message("Desired rendering library: ${RENDERING}")

# Find the appropriate packages for each windowing/rendering libraries.
if(${WINDOWING} MATCHES SFML OR ${RENDERING} MATCHES SFML)
	message("Loading SFML package")
	set(LIBS system window graphics)
	if(WIN32)
		set(LIBS ${LIBS} main)
	endif()
	find_package(SFML COMPONENTS ${LIBS} REQUIRED)
	foreach(LIB ${LIBS})
		list(APPEND FRONTEND_LIBS sfml-${LIB})
	endforeach()
endif()

add_subdirectory(Rendering)
set(RENDERING_SOURCE ${SOURCE})

add_subdirectory(Windowing)
set(WINDOWING_SOURCE ${SOURCE})

add_library(
    ${TARGET}
    ${RENDERING_SOURCE}
	${WINDOWING_SOURCE}
	Frontend.cpp
)

target_include_directories(
	${TARGET}
	PRIVATE ${OctaneGUI_INCLUDE}
)

target_link_libraries(
    ${TARGET}
    ${FRONTEND_LIBS}
)

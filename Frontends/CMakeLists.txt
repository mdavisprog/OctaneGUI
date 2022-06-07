set(TARGET Frontend)

# Default settings for each platform.
if(APPLE)
	set(WINDOWING SDL2)
	set(RENDERING Metal)
else()
	set(WINDOWING SFML)
	set(RENDERING SFML)
endif()

message("Desired windowing library: ${WINDOWING}")
message("Desired rendering library: ${RENDERING}")

set(DEFINES "")

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
	list(APPEND DEFINES SFML)
endif()

if(${WINDOWING} MATCHES SDL2 OR ${RENDERING} MATCHES SDL2)
	find_package(SDL2 REQUIRED)
	set(FRONTEND_LIBS ${SDL2_LIBRARY})
	set(INCLUDES ${SDL2_INCLUDE_DIR})
	list(APPEND DEFINES SDL2)
endif()

if(${RENDERING} MATCHES Metal)
	list(APPEND DEFINES METAL)
	set(FRONTEND_LIBS ${FRONTEND_LIBS} "-framework Metal")
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

target_compile_definitions(
	${TARGET}
	PRIVATE ${DEFINES}
)

target_include_directories(
	${TARGET}
	PRIVATE ${OctaneGUI_INCLUDE}
	PRIVATE ${INCLUDES}
)

target_link_libraries(
    ${TARGET}
    ${FRONTEND_LIBS}
)

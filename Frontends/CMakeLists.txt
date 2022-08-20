set(TARGET Frontend)

if(NOT DEFINED WINDOWING)
    if(WIN32)
        set(WINDOWING SFML)
    else()
        set(WINDOWING SDL2)
    endif()
endif()

if(NOT DEFINED RENDERING)
    if(APPLE)
        set(RENDERING Metal)
    elseif(WIN32)
        set(RENDERING SFML)
    else()
        set(RENDERING OpenGL)
    endif()
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
    set(FRONTEND_LIBS ${SDL2_LIBRARIES})
    set(INCLUDES ${SDL2_INCLUDE_DIRS})
    list(APPEND DEFINES SDL2)
endif()

if(${RENDERING} MATCHES Metal)
    list(APPEND DEFINES METAL)
    set(FRONTEND_LIBS ${FRONTEND_LIBS} "-framework Metal -framework Cocoa -framework Foundation")
endif()

if(${RENDERING} MATCHES OpenGL)
    find_package(OpenGL REQUIRED)
    list(APPEND DEFINES OPENGL)
    set(FRONTEND_LIBS ${FRONTEND_LIBS} ${OPENGL_LIBRARIES})
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

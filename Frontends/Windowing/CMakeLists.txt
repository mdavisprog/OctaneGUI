if(${WINDOWING} MATCHES SFML)
    set(SOURCE ${CMAKE_CURRENT_LIST_DIR}/SFML/Windowing.cpp)
elseif(${WINDOWING} MATCHES SDL2)
    set(SOURCE ${CMAKE_CURRENT_LIST_DIR}/SDL2/Windowing.cpp)
else()
    message(FATAL_ERROR "Windowing interface '${WINDOWING}' is not supported.")
endif()

if(WIN32)
    set(SOURCE ${SOURCE} ${CMAKE_CURRENT_LIST_DIR}/Windows/Windowing.cpp)
elseif(APPLE)
    set(SOURCE ${SOURCE} ${CMAKE_CURRENT_LIST_DIR}/Mac/Windowing.mm)
else()
    set(SOURCE ${SOURCE} ${CMAKE_CURRENT_LIST_DIR}/Linux/Windowing.cpp)
endif()

set(SOURCE
    ${SOURCE}
    ${CMAKE_CURRENT_LIST_DIR}/Windowing.cpp
    PARENT_SCOPE
)

if(${RENDERING} MATCHES SFML)
	set(SOURCE ${CMAKE_CURRENT_LIST_DIR}/SFML/Rendering.cpp)
else()
	message(FATAL_ERROR "Rendering interface '${RENDERING}' is not supported.")
endif()

set(SOURCE ${SOURCE} PARENT_SCOPE)

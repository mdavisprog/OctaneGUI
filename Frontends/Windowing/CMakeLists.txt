if(${WINDOWING} MATCHES SFML)
	set(SOURCE ${CMAKE_CURRENT_LIST_DIR}/SFML/Windowing.cpp)
else()
	message(FATAL_ERROR "Windowing interface '${WINDOWING}' is not supported.")
endif()

set(SOURCE ${SOURCE} PARENT_SCOPE)

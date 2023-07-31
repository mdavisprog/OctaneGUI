set(NO_APPS TRUE)
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../" "${CMAKE_CURRENT_BINARY_DIR}/OctaneGUI")

set(OctaneGUI_INCLUDE_DIRS ${CMAKE_CURRENT_LIST_DIR}/../Frontends/ ${CMAKE_CURRENT_LIST_DIR}/../Source)
set(OctaneGUI_LIBRARIES Frontend OctaneGUI)
set(OctaneGUI_RESOURCES_DIR ${CMAKE_CURRENT_LIST_DIR}/../Apps/Resources)
set(OctaneGUI_FOUND TRUE)

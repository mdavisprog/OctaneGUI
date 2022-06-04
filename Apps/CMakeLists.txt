set(APP_RESOURCES ${CMAKE_CURRENT_LIST_DIR}/Resources)
set(APP_FONTS ${CMAKE_CURRENT_LIST_DIR}/Resources/Roboto-Regular.ttf ${CMAKE_CURRENT_LIST_DIR}/Resources/SourceCodePro-Regular.ttf)
set(APP_INFO_IMAGE ${CMAKE_CURRENT_LIST_DIR}/Resources/info.png)
set(APP_THUMBS_UP_IMAGE ${CMAKE_CURRENT_LIST_DIR}/Resources/thumbs_up.png)
set(APP_THEMES ${CMAKE_CURRENT_LIST_DIR}/Resources/Themes)

add_subdirectory(Blank)
add_subdirectory(Hello)
add_subdirectory(Overview)
add_subdirectory(Tests)

file(
    GLOB FRONTEND_ITEMS
    LIST_DIRECTORIES TRUE
    ${FRONTEND_BIN_PATH}/*
)

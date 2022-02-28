set(APP_LAYOUTS ${CMAKE_CURRENT_LIST_DIR}/Resources/Layouts)
set(APP_FONT ${CMAKE_CURRENT_LIST_DIR}/Resources/Roboto-Regular.ttf)
set(APP_INFO_IMAGE ${CMAKE_CURRENT_LIST_DIR}/Resources/info.png)
set(APP_THUMBS_UP_IMAGE ${CMAKE_CURRENT_LIST_DIR}/Resources/thumbs_up.png)

add_subdirectory(Hello)
add_subdirectory(Overview)
add_subdirectory(Tests)

file(
    GLOB FRONTEND_ITEMS
    LIST_DIRECTORIES TRUE
    ${FRONTEND_BIN_PATH}/*
)

foreach(ITEM ${FRONTEND_ITEMS})
    get_filename_component(ITEM_NAME ${ITEM} NAME)
    if(NOT EXISTS ${BIN_DIR}/${ITEM_NAME} AND NOT ${ITEM_NAME} MATCHES "^\\..*")
        message("Copying external dependency ${ITEM_NAME} to ${BIN_DIR}.")
        file(COPY ${ITEM} DESTINATION ${BIN_DIR})
    endif()
endforeach()

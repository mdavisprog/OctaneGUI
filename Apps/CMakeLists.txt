if(NO_APPS)
    message("Not building any apps.")
    return()
endif()

set(APP_RESOURCES ${CMAKE_CURRENT_LIST_DIR}/Resources)
set(APP_FONTS ${CMAKE_CURRENT_LIST_DIR}/Resources/Roboto-Regular.ttf ${CMAKE_CURRENT_LIST_DIR}/Resources/SourceCodePro-Regular.ttf)
set(APP_INFO_IMAGE ${CMAKE_CURRENT_LIST_DIR}/Resources/info.png)
set(APP_THUMBS_UP_IMAGE ${CMAKE_CURRENT_LIST_DIR}/Resources/thumbs_up.png)
set(APP_THEMES ${CMAKE_CURRENT_LIST_DIR}/Resources/Themes)

file(GLOB
    ITEMS
    LIST_DIRECTORIES true
    RELATIVE ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/*
)

foreach(ITEM ${ITEMS})
    if(IS_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/${ITEM} AND NOT ${ITEM} STREQUAL "Resources")
        list(APPEND APPS ${ITEM})
    endif()
endforeach()

foreach(APP ${APPS})
    add_subdirectory(${APP})
    list(REMOVE_ITEM APPS ${APP})
endforeach()

file(COPY "${APP_RESOURCES}" DESTINATION "${BIN_DIR}")

unset(NO_APPS CACHE)

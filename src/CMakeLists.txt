set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    loginwindow.cpp
    loginwindow.h
    database.cpp
    database.h
    passwordmanager.cpp
    passwordmanager.h
    passworddialog.cpp
    passworddialog.h
)

# Create the library
add_library(${PROJECT_NAME}Lib STATIC
    ${PROJECT_SOURCES}
)

target_link_libraries(${PROJECT_NAME}Lib PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    OpenSSL::SSL
    OpenSSL::Crypto
) 
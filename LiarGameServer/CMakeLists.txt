cmake_minimum_required(VERSION 3.19)
project(LiarGameServer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql)
find_package(MySQL REQUIRED)

qt_standard_project_setup()

qt_add_executable(LiarGameServer
    WIN32 MACOSX_BUNDLE
    main.cpp
    server_mainwindow.cpp
    server_mainwindow.h
    server_mainwindow.ui
    db_information.h
    game_data.h
)

target_link_libraries(LiarGameServer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::Sql
        MySQL::MySQL
)

include(GNUInstallDirs)

install(TARGETS LiarGameServer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET LiarGameServer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

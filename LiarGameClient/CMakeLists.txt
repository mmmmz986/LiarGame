cmake_minimum_required(VERSION 3.19)
project(LiarGameClient LANGUAGES CXX)

# find_package에서 Network 모듈을 추가합니다.
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_resources(my_resources images.qrc)

qt_add_executable(LiarGameClient
    WIN32 MACOSX_BUNDLE
    main.cpp
    client_mainwindow.cpp
    client_mainwindow.h
    client_mainwindow.ui
    selectCategoryDialog.h
    selectCategoryDialog.cpp
    images.qrc
)

target_sources(LiarGameClient PRIVATE ${my_resources})

# target_link_libraries에 Qt::Network을 추가합니다.
target_link_libraries(LiarGameClient
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS LiarGameClient
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET LiarGameClient
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

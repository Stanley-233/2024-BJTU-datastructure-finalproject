cmake_minimum_required(VERSION 3.16)

project(traveller-linkage VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(QT_ASSUME_STDERR_HAS_CONSOLE 1)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Multimedia)

set(PROJECT_SOURCES
        res.qrc
        src/main.cpp
        src/ui/main_game_window.h
        src/ui/main_game_window.cpp
        src/ui/main_game_window.ui
        src/network/network_handler.h
        src/network/network_handler.cpp
        src/ui/login_dialog.h
        src/ui/login_dialog.cpp
        src/ui/login_dialog.ui
        src/ui/over_dialog.cpp
        src/ui/over_dialog.h
        src/ui/over_dialog.ui
        src/game/game_model.h
        src/game/game_model.cpp
        src/game/auto_conductor.cpp
        src/game/auto_conductor.h
        src/network/ranking_record.h
        src/ui/ranking_dialog.cpp
        src/ui/ranking_dialog.h
        src/ui/ranking_dialog.ui
        src/database/user_database.h
        src/database/user_database.cpp
        ${RES_FILES}
        res/icon.rc
        ${TS_FILES}
)

if (${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(traveller-linkage
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
    )
    # Define target properties for Android with Qt 6 as:
        # set_property(TARGET traveller-linkage APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
        #              ${CMAKE_CURRENT_SOURCE_DIR}/android)
    # For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else ()
    if (ANDROID)
        add_library(traveller-linkage SHARED
                ${PROJECT_SOURCES}
        )
        # Define properties for Android with Qt 5 after find_package() calls as:
            # set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else ()
        add_executable(traveller-linkage
                ${PROJECT_SOURCES}
        )
    endif ()
endif ()

target_link_libraries(traveller-linkage PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if (${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER top.bearingwall.travellerlinkage)
endif ()
set_target_properties(traveller-linkage PROPERTIES
        ${BUNDLE_ID_OPTION}
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE TRUE
        WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS traveller-linkage
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(traveller-linkage)
endif ()

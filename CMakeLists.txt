cmake_minimum_required(VERSION 3.21)

project(iina-windows
    VERSION 0.1.0
    DESCRIPTION "IINA for Windows - Modern video player"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt 6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Quick
    QuickControls2
    OpenGL
    Widgets
    Multimedia
    Network
    LinguistTools
)

# libmpv
if(WIN32)
    set(MPV_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/include")
    set(MPV_LIBRARY "${CMAKE_SOURCE_DIR}/deps/lib/mpv.lib")
    set(MPV_DLL "${CMAKE_SOURCE_DIR}/deps/lib/libmpv-2.dll")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(MPV REQUIRED IMPORTED_TARGET mpv)
    set(MPV_INCLUDE_DIR ${MPV_INCLUDE_DIRS})
    set(MPV_LIBRARY PkgConfig::MPV)
endif()

# WinSparkle (auto-update) - Windows only
if(WIN32)
    set(WINSPARKLE_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/deps/include")
    set(WINSPARKLE_LIBRARY "${CMAKE_SOURCE_DIR}/deps/lib/WinSparkle.lib")
    set(WINSPARKLE_DLL "${CMAKE_SOURCE_DIR}/deps/lib/WinSparkle.dll")
endif()

# Sources
set(CORE_SOURCES
    src/core/mpvengine.cpp
    src/core/mpvengine.h
    src/core/mpvrenderer.cpp
    src/core/mpvrenderer.h
    src/core/playercore.cpp
    src/core/playercore.h
    src/core/shortcutmanager.cpp
    src/core/shortcutmanager.h
    src/core/subtitlesearch.cpp
    src/core/subtitlesearch.h
)

set(MODEL_SOURCES
    src/models/playlistmodel.cpp
    src/models/playlistmodel.h
    src/models/chaptermodel.cpp
    src/models/chaptermodel.h
    src/models/trackmodel.cpp
    src/models/trackmodel.h
    src/models/historymodel.cpp
    src/models/historymodel.h
)

set(UTIL_SOURCES
    src/utils/logger.cpp
    src/utils/logger.h
    src/utils/config.cpp
    src/utils/config.h
    src/utils/autoupdate.cpp
    src/utils/autoupdate.h
)

set(MAIN_SOURCES
    src/main.cpp
    src/application.cpp
    src/application.h
)

# Create the executable target first (required for qt_add_qml_module)
qt_add_executable(iina-windows
    ${CORE_SOURCES}
    ${MODEL_SOURCES}
    ${UTIL_SOURCES}
    ${MAIN_SOURCES}
)

# Add QML module to the executable
qt_add_qml_module(iina-windows
    URI IINAWindows
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/VideoArea.qml
        qml/components/ControlBar.qml
        qml/components/SeekBar.qml
        qml/components/VolumeControl.qml
        qml/components/PlaylistPanel.qml
        qml/components/ChapterPanel.qml
        qml/components/HistoryPanel.qml
        qml/components/SubtitleMenu.qml
        qml/components/TitleBar.qml
        qml/components/OSD.qml
        qml/components/FilterPanel.qml
        qml/pages/SettingsPage.qml
        qml/pages/ShortcutSettings.qml
    RESOURCES
        resources/icons/iina.svg
        resources/icons/play.svg
        resources/icons/pause.svg
        resources/icons/stop.svg
        resources/icons/prev.svg
        resources/icons/next.svg
        resources/icons/volume.svg
        resources/icons/volume-mute.svg
        resources/icons/fullscreen.svg
        resources/icons/fullscreen-exit.svg
        resources/icons/playlist.svg
        resources/icons/chapters.svg
        resources/icons/history.svg
        resources/icons/subtitle.svg
        resources/icons/settings.svg
        resources/icons/screenshot.svg
        resources/icons/filter.svg
)

target_include_directories(iina-windows PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${MPV_INCLUDE_DIR}
)

if(WIN32)
    target_include_directories(iina-windows PRIVATE
        ${WINSPARKLE_INCLUDE_DIR}
    )
endif()

if(WIN32)
    target_link_libraries(iina-windows PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::OpenGL
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::Network
        ${MPV_LIBRARY}
        ${WINSPARKLE_LIBRARY}
        opengl32
        dwmapi
    )
else()
    target_link_libraries(iina-windows PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::OpenGL
        Qt6::Widgets
        Qt6::Multimedia
        Qt6::Network
        ${MPV_LIBRARY}
    )
endif()

# Windows-specific settings
if(WIN32)
    set_target_properties(iina-windows PROPERTIES
        WIN32_EXECUTABLE TRUE
        VS_DEBUGGER_ENVIRONMENT "PATH=${CMAKE_SOURCE_DIR}/deps/lib;$ENV{PATH}"
    )

    # Copy libmpv DLL to build dir
    add_custom_command(TARGET iina-windows POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MPV_DLL}"
            "$<TARGET_FILE_DIR:iina-windows>"
        COMMENT "Copying libmpv-2.dll to build directory"
    )

    # Copy WinSparkle DLL to build dir
    if(EXISTS "${WINSPARKLE_DLL}")
        add_custom_command(TARGET iina-windows POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${WINSPARKLE_DLL}"
                "$<TARGET_FILE_DIR:iina-windows>"
            COMMENT "Copying WinSparkle.dll to build directory"
        )
    endif()
endif()

# ─── Translations ───
set(TS_FILES
    translations/iina_zh_CN.ts
)

# Compile .ts -> .qm and embed as Qt resources
qt_add_translations(iina-windows
    TS_FILES ${TS_FILES}
    RESOURCE_PREFIX "/translations"
)

# Install rules
install(TARGETS iina-windows
    RUNTIME DESTINATION bin
)
if(WIN32)
    install(FILES "${MPV_DLL}" DESTINATION bin)
    if(EXISTS "${WINSPARKLE_DLL}")
        install(FILES "${WINSPARKLE_DLL}" DESTINATION bin)
    endif()
endif()

# Also install .qm files alongside the executable for filesystem fallback
install(DIRECTORY ${CMAKE_BINARY_DIR}/translations/
    DESTINATION bin/translations
    FILES_MATCHING PATTERN "*.qm"
)

cmake_minimum_required(VERSION 3.20)

project(XylarJava
    VERSION 0.1.0
    DESCRIPTION "A modern Minecraft Java launcher"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Svg Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg Network)

set(XYLARJAVA_SOURCES
    src/app/main.cpp
    src/ui/MainWindow.cpp
    src/ui/MainWindow.h
    src/launcher/LauncherController.cpp
    src/launcher/LauncherController.h
    src/launcher/LaunchPlan.cpp
    src/launcher/LaunchPlan.h
    src/minecraft/MinecraftMetadata.cpp
    src/minecraft/MinecraftMetadata.h
    src/minecraft/MinecraftVersion.cpp
    src/minecraft/MinecraftVersion.h
    src/auth/AuthManager.cpp
    src/auth/AuthManager.h
    src/instances/Instance.cpp
    src/instances/Instance.h
    src/instances/InstanceStore.cpp
    src/instances/InstanceStore.h
    src/modpacks/ModpackManager.cpp
    src/modpacks/ModpackManager.h
    src/download/DownloadManager.cpp
    src/download/DownloadManager.h
    src/java/JavaRuntimeManager.cpp
    src/java/JavaRuntimeManager.h
    src/net/ApiClient.cpp
    src/net/ApiClient.h
    src/fs/Paths.cpp
    src/fs/Paths.h
    src/util/Hash.cpp
    src/util/Hash.h
    src/util/Logger.cpp
    src/util/Logger.h
    ui/widgets/resources.qrc
)

if(WIN32)
    list(APPEND XYLARJAVA_SOURCES ui/widgets/app.rc)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_add_executable(XylarJava MANUAL_FINALIZATION WIN32 ${XYLARJAVA_SOURCES})
else()
    add_executable(XylarJava WIN32 ${XYLARJAVA_SOURCES})
endif()

target_include_directories(XylarJava PRIVATE src)

target_compile_definitions(XylarJava PRIVATE
    XYLARJAVA_VERSION="${PROJECT_VERSION}"
)

target_link_libraries(XylarJava PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Network
)

if(WIN32 AND QT_VERSION_MAJOR EQUAL 5 AND TARGET Qt5::WinMain)
    target_link_libraries(XylarJava PRIVATE Qt5::WinMain)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(XylarJava)
endif()

install(TARGETS XylarJava
    RUNTIME DESTINATION bin
    BUNDLE DESTINATION .
)

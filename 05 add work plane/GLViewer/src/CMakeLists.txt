file(GLOB_RECURSE source CONFIGURE_DEPENDS *.h *.cpp *.qrc *.ui)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)

#CGLib
set(CGLib "${CMAKE_SOURCE_DIR}/../../dependencies/CGLib")
if(WIN32)
    include_directories(${CGLib}/include)
    #find_library(GEOMETRYUTILS_LIB geo HINTS ${PROJECT_SOURCE_DIR}/testFunc/lib)
    link_directories(${CGLib}/bin/win64/Release)
    link_libraries(CGLib.lib)
endif()

set(PROJECT_SOURCES ${source})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(GLViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET GLViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(GLViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(GLViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(GLViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::OpenGLWidgets)
target_link_libraries(GLViewer PRIVATE Qt6::Gui)

target_include_directories(GLViewer PRIVATE .)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GLViewer)
endif()
set_target_properties(GLViewer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE FLASE
)

include(GNUInstallDirs)
install(TARGETS GLViewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GLViewer)
endif()

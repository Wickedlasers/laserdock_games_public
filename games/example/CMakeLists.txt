cmake_minimum_required(VERSION 3.11)

project(ldGames_Example)

# Executable
if (ANDROID)
    add_library(ldGames_Example SHARED)
else()
    add_executable(ldGames_Example MACOSX_BUNDLE WIN32)
endif()

target_sources(ldGames_Example PRIVATE
    src/main.cpp
    src/ldGamesExample.cpp
    src/ldGamesExample.h
    res/qml.qrc
    )

target_link_libraries(ldGames_Example PRIVATE ldCore ldGames)
set(QT5_MIN_VERSION 5.11)


# ---------------------------- DEPLOY  ----------------------

# resources dir
set(SOURCE_CORE_RESOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/../../core/resExt")
set(SOURCE_RESOURCES_PATH "${CMAKE_CURRENT_LIST_DIR}/../resExt")

if(WIN32)
    # Copy required dlls to output folder
    if(CMAKE_GENERATOR STREQUAL "Visual Studio 14 2015")
        set(BUILD_SUB_PATH "${CMAKE_BUILD_TYPE}")
    else()
        set(BUILD_SUB_PATH "")
    endif()

    set(EXEDIR_PATH ${CMAKE_CURRENT_BINARY_DIR})

    set(LDCORELIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/ldCore.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LDCORELIB_PATH} ${EXEDIR_PATH})

    set(LIBUSB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../lib/3rdparty/laserdocklib/libusb/libusb-1.0.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LIBUSB_PATH} ${EXEDIR_PATH})

    set(LASERDOCKLIB_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/3rdparty/laserdocklib${BUILD_SUB_PATH}/laserdocklib.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${LASERDOCKLIB_PATH} ${EXEDIR_PATH})

    set(OPENLASE_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/3rdparty/openlase${BUILD_SUB_PATH}/OpenLase.dll")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${OPENLASE_PATH} ${EXEDIR_PATH})

    if(LD_CORE_ENABLE_LIBLZR)
        set(LIBLZR_PATH "${CMAKE_CURRENT_BINARY_DIR}/../lib/3rdparty/liblzr${BUILD_SUB_PATH}/liblzr.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${LIBLZR_PATH} ${EXEDIR_PATH}
            )
    endif()

    if(LD_CORE_ENABLE_OPENCV)
        #opencv
        include(../lib/3rdparty/opencv/CMakeLists.txt)

        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(DEBUG_SUFFIX "d")
        else()
            set(DEBUG_SUFFIX "")
        endif()

        set(OPENCVCORE_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_core320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVCORE_PATH} ${EXEDIR_PATH})
        set(OPENCVHIGHGUI_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_highgui320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVHIGHGUI_PATH} ${EXEDIR_PATH})
        set(OPENCVIMGCODECS_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_imgcodecs320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVIMGCODECS_PATH} ${EXEDIR_PATH})
        set(OPENCVIMGPROC_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_imgproc320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVIMGPROC_PATH} ${EXEDIR_PATH})
        set(OPENCVVIDEOIO_PATH "${OpenCV_DIR}/x86/vc14/bin/opencv_videoio320${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${OPENCVVIDEOIO_PATH} ${EXEDIR_PATH})
    endif()

    # copy resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_RESOURCES_PATH} "${EXEDIR_PATH}/Resources" || true) # "|| true" - trick to return always no error code

    # copy core resources
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_CORE_RESOURCES_PATH} "${EXEDIR_PATH}/Resources" || true) # "|| true" - trick to return always no error code
endif()

if(APPLE)
    # set correct bundle prefix for Xcode generator
    if(CMAKE_GENERATOR STREQUAL Xcode)
        set(BUNDLE_SUB_PATH "${CMAKE_CURRENT_BINARY_DIR}/\${CONFIGURATION}")
    else()
        set(BUNDLE_SUB_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif()
    # set bundle pathes
    set(BUNDLE_PATH "${BUNDLE_SUB_PATH}/${PROJECT_NAME}.app")
    set(BUNDLE_RESOURCES_PATH "${BUNDLE_PATH}/Contents/Resources")

    # copy resourceas
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -R -n ${SOURCE_RESOURCES_PATH}/* ${BUNDLE_RESOURCES_PATH} || true) # "|| true" - trick to return always no error code

    # copy resourceas
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp -R -n ${SOURCE_CORE_RESOURCES_PATH}/* ${BUNDLE_RESOURCES_PATH} || true) # "|| true" - trick to return always no error code

endif()

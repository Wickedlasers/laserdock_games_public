cmake_minimum_required(VERSION 3.0)

project(ldTetris)

add_library(${PROJECT_NAME})

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_LIST_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE ldCore)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(${PROJECT_NAME} PRIVATE
   ${SRCDIR}/ldTetris.cpp
   ${SRCDIR}/ldTetris.h
   ${SRCDIR}/vis/ldTetrisVisualizer.cpp
   ${SRCDIR}/vis/ldTetrisVisualizer.h
    )

cmake_minimum_required(VERSION 3.0)

set(CMAKE_AUTOMOC OFF)

add_library(ldRazer)

target_include_directories(ldRazer PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(ldRazer PRIVATE ldCore)

set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/ldRazer)

target_sources(ldRazer PRIVATE
   ${SRCDIR}/ldRazer.h
   ${SRCDIR}/ldRazer.cpp
   ${SRCDIR}/ldVec2.h
   ${SRCDIR}/ldAABB.h
   ${SRCDIR}/ldDisc.h
   ${SRCDIR}/ldPolygon.h
   ${SRCDIR}/ldHull.h
   ${SRCDIR}/ldLine.h
   ${SRCDIR}/ldParticle.h
   ${SRCDIR}/ldQueries.cpp
   ${SRCDIR}/ldQueries.h
   ${SRCDIR}/ldRay.h
   ${SRCDIR}/ldMatrix.h
   ${SRCDIR}/ldVector.h
   ${SRCDIR}/ldRenderBatch.h
   ${SRCDIR}/ldPhysicsEngine.cpp
   ${SRCDIR}/ldPhysicsEngine.h
   ${SRCDIR}/ldBody.h
   ${SRCDIR}/ldAIManager.cpp
   ${SRCDIR}/ldAIManager.h
   ${SRCDIR}/ldWorld.cpp
   ${SRCDIR}/ldWorld.h
   ${SRCDIR}/ldAIManager.cpp
   ${SRCDIR}/ldAIManager.h
   ${SRCDIR}/ldSpline.cpp
   ${SRCDIR}/ldSpline.h
   ${SRCDIR}/ldLaserFont.cpp
   ${SRCDIR}/ldLaserFont.h
   ${SRCDIR}/ldLODModel.h
   ${SRCDIR}/ldLineClipper.h
   ${SRCDIR}/ldLineClipper.cpp
   ${SRCDIR}/ldQuadTree.h
   ${SRCDIR}/ldColourSpline.h
    )

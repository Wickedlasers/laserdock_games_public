cmake_minimum_required(VERSION 3.0)

option(LD_GAMES_BUILD_EXAMPLE "Build Games Example" OFF)

project(ldGames)
add_library(${PROJECT_NAME})

set(INCDIR ${CMAKE_CURRENT_LIST_DIR}/include)
set(SRCDIR ${CMAKE_CURRENT_LIST_DIR}/src)

target_sources(${PROJECT_NAME} PRIVATE
    ${SRCDIR}/ldGameManager.cpp
    ${INCDIR}/ldGames/ldGameManager.h
    )

add_subdirectory(ldRazer)

target_include_directories(${PROJECT_NAME} PRIVATE ${SRCDIR})
target_include_directories(${PROJECT_NAME} PUBLIC ${INCDIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ldCore)

find_package(Qt5 COMPONENTS Core REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC Qt5::Core)

add_subdirectory(Games/ArmorAttack)
target_link_libraries(ldGames PRIVATE ldArmorAttack)

add_subdirectory(Games/Asteroids)
target_link_libraries(ldGames PRIVATE ldAsteroids)

add_subdirectory(Games/BlackWidow)
target_link_libraries(ldGames PRIVATE ldBlackWidow)

add_subdirectory(Games/Breakout)
target_link_libraries(ldGames PRIVATE ldBreakout)

add_subdirectory(Games/Defender)
target_link_libraries(ldGames PRIVATE ldDefender)

add_subdirectory(Games/DesertPunk)
target_link_libraries(ldGames PRIVATE ldDesertPunk)

add_subdirectory(Games/DemoPlayer)
target_link_libraries(ldGames PRIVATE ldDemoPlayer)

add_subdirectory(Games/DonkeyKong)
target_link_libraries(ldGames PRIVATE ldDonkeyKong)

add_subdirectory(Games/FlappyBird)
target_link_libraries(ldGames PRIVATE ldFlappyBird)

add_subdirectory(Games/Frogger)
target_link_libraries(ldGames PRIVATE ldFrogger)

add_subdirectory(Games/Genius)
target_link_libraries(ldGames PRIVATE ldGenius)

add_subdirectory(Games/GeometryWars)
target_link_libraries(ldGames PRIVATE ldGeometryWars)

add_subdirectory(Games/GrooveCoaster)
target_link_libraries(ldGames PRIVATE ldGrooveCoaster)

add_subdirectory(Games/Hexagon)
target_link_libraries(ldGames PRIVATE ldHexagon)

add_subdirectory(Games/HyperChase)
target_link_libraries(ldGames PRIVATE ldHyperChase)

add_subdirectory(Games/LaserRacer)
target_link_libraries(ldGames PRIVATE ldLaserRacer)

add_subdirectory(Games/LunarLander)
target_link_libraries(ldGames PRIVATE ldLunarLander)

add_subdirectory(Games/MissileCommand)
target_link_libraries(ldGames PRIVATE ldMissileCommand)

#add_subdirectory(Games/OldAsteroids)
#target_link_libraries(ldGames PRIVATE ldOldAsteroids)

add_subdirectory(Games/OldMissileCommand)
target_link_libraries(ldGames PRIVATE ldOldMissileCommand)

add_subdirectory(Games/OldPacMan)
target_link_libraries(ldGames PRIVATE ldOldPacMan)

add_subdirectory(Games/OldSnake)
target_link_libraries(ldGames PRIVATE ldOldSnake)

#add_subdirectory(Games/OldTempest)
#target_link_libraries(ldGames PRIVATE ldOldTempest)

add_subdirectory(Games/OldTetris)
target_link_libraries(ldGames PRIVATE ldOldTetris)

add_subdirectory(Games/PacMan)
target_link_libraries(ldGames PRIVATE ldPacMan)

add_subdirectory(Games/Pong)
target_link_libraries(ldGames PRIVATE ldPong)

add_subdirectory(Games/RiverRaid)
target_link_libraries(ldGames PRIVATE ldRiverRaid)

add_subdirectory(Games/ScorchedEarth)
target_link_libraries(ldGames PRIVATE ldScorchedEarth)

add_subdirectory(Games/Serpent)
target_link_libraries(ldGames PRIVATE ldSerpent)

add_subdirectory(Games/ShootEmUp)
target_link_libraries(ldGames PRIVATE ldShootEmUp)

add_subdirectory(Games/SkyRoads)
target_link_libraries(ldGames PRIVATE ldSkyRoads)

add_subdirectory(Games/SpiralFighter)
target_link_libraries(ldGames PRIVATE ldSpiralFighter)

add_subdirectory(Games/Lero)
target_link_libraries(ldGames PRIVATE ldLero)

add_subdirectory(Games/Stacker)
target_link_libraries(ldGames PRIVATE ldStacker)

add_subdirectory(Games/StarCastle)
target_link_libraries(ldGames PRIVATE ldStarCastle)

add_subdirectory(Games/StarHawk)
target_link_libraries(ldGames PRIVATE ldStarHawk)

add_subdirectory(Games/StarShip)
target_link_libraries(ldGames PRIVATE ldStarShip)

add_subdirectory(Games/Tempest)
target_link_libraries(ldGames PRIVATE ldTempest)

add_subdirectory(Games/Tetris)
target_link_libraries(ldGames PRIVATE ldTetris)

add_subdirectory(Games/Wormhole)
target_link_libraries(ldGames PRIVATE ldWormhole)

if(LD_GAMES_BUILD_EXAMPLE)
    add_subdirectory(example)
endif()

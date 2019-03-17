#include <ldGames/ldGameManager.h>

#include <QtCore/QtDebug>
#include <QtQml/QQmlEngine>

#include "ldArmorAttack.h"
#include "ldAsteroids.h"
#include "ldBlackWidow.h"
#include "ldBreakout.h"
#include "ldDefender.h"
#include "ldDesertPunk.h"
#include "ldDemoPlayer.h"
#include "ldDonkeyKong.h"
#include "ldFlappyBird.h"
#include "ldFrogger.h"
#include "ldHexagon.h"
#include "ldHyperChase.h"
#include "ldGeometryWars.h"
#include "ldGenius.h"
#include "ldGrooveCoaster.h"
#include "ldLaserRacer.h"
#include "ldLunarLander.h"
#include "ldMissileCommand.h"
#include "ldOldMissileCommand.h"
#include "ldOldPacMan.h"
#include "ldOldSnake.h"
#include "ldOldTetris.h"
#include "ldPacMan.h"
#include "ldPong.h"
#include "ldRiverRaid.h"
#include "ldScorchedEarth.h"
#include "ldScorchedEarthMulti.h"
#include "ldShootEmUp.h"
#include "ldSerpent.h"
#include "ldSkyRoads.h"
#include "ldSpiralFighterGame.h"
#include "ldStacker.h"
#include "ldStarCastle.h"
#include "ldStarHawk.h"
#include "ldStarShip.h"
#include "ldTempest.h"
#include "ldTetris.h"
#include "ldWormhole.h"
#include "ldLeroGame.h"

void ldGameManager::initResources()
{
    ldDemoPlayer::initResources();
}

void ldGameManager::registerMetaTypes()
{
    qRegisterMetaType<ldAbstractGame*>("ldAbstractGame*");
    qRegisterMetaType<QList<ldAbstractGame*>>("QList<ldAbstractGame*>");

    ldAbstractGame::registerMetaTypes();
}

ldGameManager::ldGameManager(QObject *parent)
    : QObject(parent)
    , m_activeGame(nullptr)
    , m_isActive(false)
    , m_currentGameIndex(0)
    , m_soundEnabled(ldGameManager::DEFAULT_SOUND_ENABLED)
    , m_gamepad(new ldGamepad(this))
{
    qmlRegisterType<ldGamepad>();
    qmlRegisterType<ldAbstractGame>();

    m_games.push_back(QVariant::fromValue(new ldLeroGame(this)));
    m_games.push_back(QVariant::fromValue(new ldLaserRacer(this)));
    m_games.push_back(QVariant::fromValue(new ldTempest(this)));
    m_games.push_back(QVariant::fromValue(new ldMissileCommand(this)));
    m_games.push_back(QVariant::fromValue(new ldOldMissileCommand(this)));
    m_games.push_back(QVariant::fromValue(new ldDefender(this)));
    m_games.push_back(QVariant::fromValue(new ldStarCastle(this)));
    m_games.push_back(QVariant::fromValue(new ldTetris(this)));
    m_games.push_back(QVariant::fromValue(new ldOldTetris(this)));
    m_games.push_back(QVariant::fromValue(new ldBreakout(this)));
    m_games.push_back(QVariant::fromValue(new ldAsteroids(this)));
    m_games.push_back(QVariant::fromValue(new ldHexagon(this)));
    m_games.push_back(QVariant::fromValue(new ldOldSnake(this)));
    m_games.push_back(QVariant::fromValue(new ldSerpent(this)));
    m_games.push_back(QVariant::fromValue(new ldFlappyBird(this)));
    m_games.push_back(QVariant::fromValue(new ldPong(this)));
    m_games.push_back(QVariant::fromValue(new ldPacMan(this)));
    m_games.push_back(QVariant::fromValue(new ldOldPacMan(this)));
    m_games.push_back(QVariant::fromValue(new ldShootEmUp(this)));
    m_games.push_back(QVariant::fromValue(new ldGenius(this)));
    m_games.push_back(QVariant::fromValue(new ldStacker(this)));
    m_games.push_back(QVariant::fromValue(new ldFrogger(this)));
    m_games.push_back(QVariant::fromValue(new ldRiverRaid(this)));
    m_games.push_back(QVariant::fromValue(new ldScorchedEarth(this)));
    m_games.push_back(QVariant::fromValue(new ldScorchedEarthMulti(this)));
    m_games.push_back(QVariant::fromValue(new ldStarHawk(this)));
    m_games.push_back(QVariant::fromValue(new ldStarShip(this)));
    m_games.push_back(QVariant::fromValue(new ldArmorAttack(this)));
    m_games.push_back(QVariant::fromValue(new ldSpiralFighterGame(this)));
    m_games.push_back(QVariant::fromValue(new ldGrooveCoaster(this)));
    m_games.push_back(QVariant::fromValue(new ldDonkeyKong(this)));
    m_games.push_back(QVariant::fromValue(new ldHyperChase(this)));
    m_games.push_back(QVariant::fromValue(new ldDesertPunk(this)));
    m_games.push_back(QVariant::fromValue(new ldGeometryWars(this)));
    m_games.push_back(QVariant::fromValue(new ldBlackWidow(this)));
    m_games.push_back(QVariant::fromValue(new ldLunarLander(this)));
    m_games.push_back(QVariant::fromValue(new ldWormhole(this)));
    m_games.push_back(QVariant::fromValue(new ldSkyRoads(this)));

    // Note: Always make ldDemoPlayer the last entry in m_games
    m_games.push_back(QVariant::fromValue(new ldDemoPlayer(this)));

    // setup connections
    for(const QVariant &gameVar : m_games) {
        ldAbstractGame *game = gameVar.value<ldAbstractGame*>();
        game->setSoundEnabled(m_soundEnabled);
        connect(this, &ldGameManager::soundEnabledChanged, game, &ldAbstractGame::setSoundEnabled);
    }

    connect(this, &ldGameManager::currentGameIndexChanged, this, &ldGameManager::activate);

    connect(this, &ldGameManager::isActiveChanged, m_gamepad, &ldGamepad::update_isActive);
    connect(this, &ldGameManager::isActiveChanged, this, [&](bool isActive) {
       if(isActive)
           activate(m_currentGameIndex);
       else
           if(m_activeGame) m_activeGame->set_isActive(false);
    });
    connect(m_gamepad, &ldGamepad::axisLeftXChanged, this, [&](double axisX) {
       if(m_activeGame) m_activeGame->moveX(axisX);
    });
    connect(m_gamepad, &ldGamepad::axisLeftYChanged,  this, [&](double axisY) {
        if(m_activeGame) m_activeGame->moveY(axisY);
     });
    connect(m_gamepad, &ldGamepad::axisRightXChanged, this, [&](double axisX) {
       if(m_activeGame) m_activeGame->moveRightX(axisX);
    });
    connect(m_gamepad, &ldGamepad::axisRightYChanged,  this, [&](double axisY) {
        if(m_activeGame) m_activeGame->moveRightY(axisY);
     });
    connect(m_gamepad, &ldGamepad::buttonPressed, this, [&](ldGamepad::Button button, bool pressed) {
        if(m_activeGame) m_activeGame->get_gamepadCtrl()->pressButton(button, pressed);
     });
}

ldGameManager::~ldGameManager()
{
}

bool ldGameManager::eventFilter(QObject *obj, QEvent *ev)
{
    return (m_activeGame != nullptr) ? m_activeGame->eventFilter(obj, ev) : QObject::eventFilter(obj, ev);
}


void ldGameManager::next()
{
    int nextGameIndex = m_currentGameIndex + 1;
    if(nextGameIndex >= m_games.size()) {
        nextGameIndex = 0;
    }

    set_currentGameIndex(nextGameIndex);
}

void ldGameManager::previous()
{
    int nextGameIndex = m_currentGameIndex - 1;
    if(nextGameIndex < 0) {
        nextGameIndex = m_games.size() - 1;
    }
    set_currentGameIndex(nextGameIndex);
}

void ldGameManager::resetSettings()
{
    for(const QVariant &gameVar : m_games) {
        ldAbstractGame *game = gameVar.value<ldAbstractGame*>();
        game->reset();
    }
}

void ldGameManager::activate(int game)
{
    if(!m_isActive)
        return;

    if(m_activeGame) m_activeGame->set_isActive(false);

    update_activeGame(m_games[game].value<ldAbstractGame*>());

    m_activeGame->set_isActive(true);
}

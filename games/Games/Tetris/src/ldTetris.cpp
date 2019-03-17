#include "ldTetris.h"

#include <QtGui/QtEvents>

#include "vis/ldTetrisVisualizer.h"

ldTetris::ldTetris(QObject *parent)
    : ldAbstractGame("tetrisGame", tr("Lasertris A"), parent)
    , m_visualizer(new ldTetrisVisualizer)
{
    m_keyDescriptions << "Left" << "Left";
    m_keyDescriptions << "Right" << "Right";
    m_keyDescriptions << "Up" << "Rotate";
    m_keyDescriptions << "Down" << "Pull Down";

    QMap<ldGamepad::Button, Qt::Key> keyMap;
    keyMap[ldGamepad::Button::Left] = Qt::Key_Left;
    keyMap[ldGamepad::Button::Right] = Qt::Key_Right;
    keyMap[ldGamepad::Button::Down] = Qt::Key_Down;
    keyMap[ldGamepad::Button::B] = Qt::Key_Up;
    get_gamepadCtrl()->init(keyMap, m_keyDescriptions);

    connect(m_visualizer.data(), &ldTetrisVisualizer::finished, this, [&]() {
       set_isPaused(false);
       set_isPlaying(false);
    });
}

ldTetris::~ldTetris() {
}

bool ldTetris::handleKeyEvent(QKeyEvent *keyEvent) {
    if(keyEvent->type() == QEvent::KeyPress) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            // Move Left
            m_visualizer->moveLeft(true);
            break;
        case Qt::Key_Right:
            // Move Right
            m_visualizer->moveRight(true);
            break;
        case Qt::Key_Down:
            // Pull Down
            m_visualizer->moveDown(true);
            break;
        case Qt::Key_Up:
            // Rotate
            m_visualizer->moveRotate(true);
            break;
        default:
            break;
        }
        return true;
    } else if(keyEvent->type() == QEvent::KeyRelease) {
        switch (keyEvent->key()) {
        case Qt::Key_Left:
            // Move Left
            m_visualizer->moveLeft(false);
            break;
        case Qt::Key_Right:
            // Move Right
            m_visualizer->moveRight(false);
            break;
        case Qt::Key_Down:
            // Pull Down
            m_visualizer->moveDown(false);
            break;
        case Qt::Key_Up:
            // Rotate
            m_visualizer->moveRotate(false);
            break;
        default:
            break;
        }
        return true;
    }

    return false;
}

ldAbstractGameVisualizer *ldTetris::getGameVisualizer() const {
    return m_visualizer.data();
}

#ifndef LDTetris_H
#define LDTetris_H

#include <ldCore/Games/ldAbstractGame.h>

class ldTetrisVisualizer;

/**
 * @brief The ldTetris class
 * qsTr("< : Left\n> : Right\nv  : Pull Down\n^  : Rotate");
 */
class ldTetris : public ldAbstractGame
{
    Q_OBJECT

public:
    explicit ldTetris(QObject *parent = 0);
    virtual ~ldTetris();

private:
    virtual bool handleKeyEvent(QKeyEvent *keyEvent) override;
    virtual ldAbstractGameVisualizer *getGameVisualizer() const override;

    QScopedPointer<ldTetrisVisualizer> m_visualizer;
};

#endif // LDTetris_H

#ifndef LDGAMEMANAGER_H
#define LDGAMEMANAGER_H

#include <QtCore/QVariantList>

#include <QQmlHelpers>

class ldAbstractGame;
class ldGamepad;

class ldGameManager : public QObject
{
    Q_OBJECT

    QML_CONSTANT_PROPERTY(QVariantList, games)
    QML_CONSTANT_PROPERTY(QStringList, titles)

    QML_READONLY_PROPERTY(ldAbstractGame*, activeGame)

    QML_READONLY_PROPERTY(bool, isActive)

    QML_WRITABLE_PROPERTY(int, currentGameIndex)
    QML_WRITABLE_PROPERTY(bool, soundEnabled)

    QML_CONSTANT_PROPERTY(ldGamepad*, gamepad)

public:
    static void initResources();

    static const bool DEFAULT_SOUND_ENABLED = false;

    static void registerMetaTypes();

    explicit ldGameManager(QObject *parent = 0);
    virtual ~ldGameManager();

    /** QObject impl */
    virtual bool eventFilter(QObject *obj, QEvent *ev) override;

public slots:
    void next();
    void previous();

    void resetSettings();

private:
    void activate(int game);

private:
    QObject *m_window = nullptr;
};

#endif // LDGAMEMANAGER_H



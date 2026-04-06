#pragma once

#include <QObject>
#include <QSoundEffect>
#include <QUrl>
#include <unordered_map>
#include <string>
#include <QDebug>

// This is basically a class which is instantiated once and never again and everyone can access
// it through AudioManager::instance() so all sound effects are handled in one place
class AudioManager : public QObject
{
public:
    static AudioManager &instance()
    {
        static AudioManager _instance;
        return _instance;
    }

    void playSound(const std::string &key)
    {
        auto it = soundTable.find(key);
        if (it != soundTable.end())
        {
            it->second->play();
        }
        else
        {
            qWarning() << "Sound key not found:" << QString::fromStdString(key);
        }
    }

    AudioManager(const AudioManager &) = delete;
    AudioManager &operator=(const AudioManager &) = delete;

private:
    AudioManager(QObject *parent = nullptr) : QObject(parent)
    {
        QSoundEffect *grassWalk = new QSoundEffect(this);
        grassWalk->setSource(QUrl("qrc:/resources/sound-effects/GrassWalk.wav"));
        grassWalk->setVolume(0.3f);
        soundTable["GrassWalk"] = grassWalk;

        QSoundEffect *swordSwing = new QSoundEffect(this);
        swordSwing->setSource(QUrl("qrc:/resources/sound-effects/SwordSwing.wav"));
        swordSwing->setVolume(0.5f);
        soundTable["SwordSwing"] = swordSwing;

        QSoundEffect *damageTaken = new QSoundEffect(this);
        damageTaken->setSource(QUrl("qrc:/resources/sound-effects/DamageTaken.wav"));
        damageTaken->setVolume(0.5f);
        soundTable["DamageTaken"] = damageTaken;

        QSoundEffect *gameOver = new QSoundEffect(this);
        gameOver->setSource(QUrl("qrc:/resources/sound-effects/GameOver.wav"));
        gameOver->setVolume(2);
        soundTable["GameOver"] = gameOver;
    }

    std::unordered_map<std::string, QSoundEffect *> soundTable;
};
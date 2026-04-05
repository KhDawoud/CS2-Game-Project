#include "player.hpp"
#include "characterstats.hpp"
#include <QGraphicsPixmapItem>
#include <QObject>
#include <QPixmap>
#include <QPainter>

CharacterStats::CharacterStats()
{
    health = 0.0f;
    mana = 0.0f;
    stamina = 0.0f;
    spriteSheet.load(":/resources/ui-elements/character_panel.png");
    setScale(1.3);
}
void CharacterStats::setPlayer(Player *player)
{
    this->player = player;
    updateBars();
}
void CharacterStats::updateValues()
{
    if (!player)
        return;

    this->health = player->getHealth();
    this->mana = player->getMana();
    this->stamina = player->getStamina();
}
void CharacterStats::updateBars()
{
    updateValues();
    if (!player || spriteSheet.isNull())
        return;

    // we get each element we need from the sprite sheet seperately
    QRect basePanelRect(0, 0, 90, 32);
    QRect healthBarFullRect(14, 138, 52, 2); // Red
    QRect manaBarFullRect(16, 143, 42, 2);   // Blue
    QRect staminaFullRect(15, 148, 38, 2);   // Green

    // where each bar starts relative to the panel
    QPoint healthOffsetOnPanel(30, 10);
    QPoint manaOffsetOnPanel(32, 15);
    QPoint staminaOffsetOnPanel(31, 20);

    QPixmap finalImage(basePanelRect.size());
    finalImage.fill(Qt::transparent);

    QPainter painter(&finalImage);

    // draw panel
    painter.drawPixmap(0, 0, spriteSheet.copy(basePanelRect));

    float healthPercent = std::max(0.0f, this->health / 100.0f);
    float manaPercent = std::max(0.0f, this->mana / 100.0f);
    float staminaPercent = std::max(0.0f, this->stamina / 100.0f);

    // scale healthbar by current health and draw it
    int currentHealthWidth = static_cast<int>(healthBarFullRect.width() * healthPercent);
    if (currentHealthWidth > 0)
    {
        QRect currentHealthRect = healthBarFullRect;
        currentHealthRect.setWidth(currentHealthWidth);
        painter.drawPixmap(healthOffsetOnPanel, spriteSheet.copy(currentHealthRect));
    }

    // scale manabar by current mana and draw it
    int currentManaWidth = static_cast<int>(manaBarFullRect.width() * manaPercent);
    if (currentManaWidth > 0)
    {
        QRect currentManaRect = manaBarFullRect;
        currentManaRect.setWidth(currentManaWidth);
        painter.drawPixmap(manaOffsetOnPanel, spriteSheet.copy(currentManaRect));
    }

    // scale staminabar by current stamina and draw it
    int currentStaminaWidth = static_cast<int>(staminaFullRect.width() * staminaPercent);
    if (currentStaminaWidth > 0)
    {
        QRect currentStaminaRect = staminaFullRect;
        currentStaminaRect.setWidth(currentStaminaWidth);
        painter.drawPixmap(staminaOffsetOnPanel, spriteSheet.copy(currentStaminaRect));
    }

    painter.end();

    setPixmap(finalImage);
}
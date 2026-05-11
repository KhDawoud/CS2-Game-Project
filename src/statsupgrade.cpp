#include "statsupgrade.hpp"

Statsupgrade::Statsupgrade(QWidget *parent, Stats stats, int Level) : BaseWindow(parent) {
    setFixedSize(500,500);
    QFont font(pixelFontFamily, 16);

    QLabel *header = new QLabel("STATS INCREASED!", this);
    header->setFont(QFont(pixelFontFamily, 20, QFont::Bold));
    header->setStyleSheet("color: black;");
    header->setAlignment(Qt::AlignCenter);

    QString statText = QString(
                           "HEALTH:  %1 -> %2\n\n"
                           "MANA:    %3 -> %4\n\n"
                           "STAMINA: %5 -> %6"
                           )
                           .arg(stats.oldhealth).arg(stats.health)
                           .arg(stats.oldmana).arg(stats.mana)
                           .arg(stats.oldstamina).arg(stats.stamina);
    QLabel *display = new QLabel(statText, this);
    display->setFont(font);
    display->setStyleSheet("color: black;");
    display->setAlignment(Qt::AlignCenter);

    QPushButton *okBtn = new QPushButton("CONTINUE", this);
    okBtn->setFont(font);
    okBtn->setStyleSheet("background-color: #4CAF50; color: black; border-radius: 10px;");
    okBtn->setMinimumHeight(60);

    mainLayout->addWidget(header);
    mainLayout->addWidget(display);
    if(Level ==1){
        QLabel *skill = new QLabel("New DASH SKILL UNLOCKED!");
        skill->setFont(font);
        skill->setStyleSheet("color: black;");
        skill->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(skill);
    }
    mainLayout->addWidget(okBtn);

    connect(okBtn, &QPushButton::clicked, this, &QDialog::accept);
}

#ifndef LEVELINTRO_H
#define LEVELINTRO_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class LevelIntro : public QWidget
{
    Q_OBJECT
public:
    explicit LevelIntro(QWidget *parent = nullptr);

signals:
    void introFinished();

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LEVELINTRO_H
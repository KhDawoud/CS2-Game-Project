#ifndef LEVELINTRO_H
#define LEVELINTRO_H
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
class LevelIntro : public QDialog
{
public:
    LevelIntro(QWidget *);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LEVELINTRO_H

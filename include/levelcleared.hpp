#ifndef LEVELCLEARED_H
#define LEVELCLEARED_H
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
class LevelCleared : public QDialog
{
public:
    LevelCleared(QWidget *);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // LEVELCLEARED_H

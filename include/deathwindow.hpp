#ifndef DEATHWINDOW_H
#define DEATHWINDOW_H
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
class DeathWindow : public QDialog
{
public:
    DeathWindow(QWidget *);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // DEATHWINDOW_H

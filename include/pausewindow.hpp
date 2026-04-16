#ifndef PAUSEWINDOW_H
#define PAUSEWINDOW_H
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QApplication>
class pausewindow: public QDialog
{
public:
    pausewindow(QWidget*);
protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // PAUSEWINDOW_H

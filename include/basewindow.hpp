#ifndef BASEWINDOW_HPP
#define BASEWINDOW_HPP

#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFontDatabase>
#include <QPainter>

class BaseWindow : public QDialog {
    Q_OBJECT
public:
    explicit BaseWindow(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    QString pixelFontFamily;
    QVBoxLayout *mainLayout;
};

#endif // BASEWINDOW_HPP

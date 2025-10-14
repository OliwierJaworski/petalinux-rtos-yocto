#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gpio.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    led_simple *leds;

private slots:
    void on_led0_tick_toggled(bool checked);

    void on_led3_tick_toggled(bool checked);

    void on_led1_tick_toggled(bool checked);

    void on_led2_tick_toggled(bool checked);

    void on_led3_tick_2_toggled(bool checked);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

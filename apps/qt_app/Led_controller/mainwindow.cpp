#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    leds = new led_simple("/dev/uio0",0x0,0x4);
}

MainWindow::~MainWindow()
{
    delete leds;
    delete ui;
}

void MainWindow::on_led0_tick_toggled(bool checked)
{

    leds->writeLedx((checked<<0), 0);
}

void MainWindow::on_led1_tick_toggled(bool checked)
{
    leds->writeLedx((checked<<1), 0);
}

void MainWindow::on_led2_tick_toggled(bool checked)
{
    leds->writeLedx((checked<<2), 0);
}

void MainWindow::on_led3_tick_toggled(bool checked)
{
    leds->writeLedx((checked<<3), 0);
}

void MainWindow::on_led3_tick_2_toggled(bool checked)
{
    leds->writeLedx(0, 0);
}


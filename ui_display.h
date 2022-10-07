#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H

#include <QMainWindow>
#include <QModbusClient>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <DataProcessing.h>
#include <QTimer>
#include <qcustomplot.h>
class QModbusClient;
class QModbusReply;
namespace Ui {
class UI_display;
}

class DataProcessing;
class UI_display : public QMainWindow
{
    Q_OBJECT

public:
    explicit UI_display(QWidget *parent = 0);
    ~UI_display();

public:
    QModbusDataUnit readRequest() const;


private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();
    void on_start_clicked();
    void on_pause_clicked();
    void customsplot_init();
    void readReady();
    void realtimedatashow();
    void ShowInit();
    void begin_collect();

private:
    Ui::UI_display *ui;
    QModbusClient *modbusDevice;
    DataProcessing* DataProcces;

    QThread * m_ProDataThread;

    double data1;         //data
    double data2;
    double data3;
    double data4;
    double data5;
    double data6;
    double data7;
    double data8;
    QString dir;
    QString fileName;
    QFile file;
    //定时器
    QTimer *_pTimerUpdate;
    QElapsedTimer _elapsedTimer;        //之间计时
    int _fps;
    int _interval;
    bool _pause;
    double _lastPointKey;
};

#endif // UI_DISPLAY_H

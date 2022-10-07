#include "ui_display.h"
#include "ui_ui_display.h"

#include <QStatusBar>
#include <QTextStream>
#include <QFile>
#include <QDataStream>
#include <QUrl>
#include <QDate>
#include <QDebug>
#include <DataProcessing.h>
#include <QThread>
#include <qobject.h>



union Uint16Tofloat
{
    float fValue;
    uint16_t arrUint16[2];
};


UI_display::UI_display(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UI_display)
    ,modbusDevice(nullptr)
{
    ui->setupUi(this);

    //实例化一个数据处理对象
    DataProcessing* DataProcces = new DataProcessing();

//    //新建一个数据处理线程
    m_ProDataThread = new QThread;

//    //将工作类挪入线程中
    DataProcces->moveToThread(m_ProDataThread);

    m_ProDataThread->start();




    //设置TCP
    if (modbusDevice)
    {
        modbusDevice->disconnectDevice();
        delete modbusDevice;
        modbusDevice = nullptr;
    }
    modbusDevice = new QModbusTcpClient(this);
    if (ui->lineEdit->text().isEmpty())
        ui->lineEdit->setText(QLatin1Literal("127.0.0.1:1502"));


    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        statusBar()->showMessage(modbusDevice->errorString(), 5000);
    });

    if (!modbusDevice)
    {
        //ui->connectButton->setDisabled(true);
        statusBar()->showMessage(tr("Could not create Modbus client."), 5000);
    }



    //初始化图表
    ShowInit();


    // 实时更新
    _pTimerUpdate = new QTimer(this);
    connect(_pTimerUpdate, SIGNAL(timeout()), this, SLOT(customsplot_init()));
}


UI_display::~UI_display()
{
    delete ui;
}


// 一秒运行一次
void UI_display::customsplot_init()
{
    qDebug() << "in" << endl;
    begin_collect();
    //绘图
    realtimedatashow();

    QString tmp1,tmp2,tmp3,tmp4,tmp5,tmp6,tmp7,tmp8;
    tmp1 = QString("%1").arg(data1);
    tmp2 = QString("%1").arg(data2);
    tmp3 = QString("%1").arg(data3);
    tmp4 = QString("%1").arg(data4);
    tmp5 = QString("%1").arg(data5);
    tmp6 = QString("%1").arg(data6);
    tmp7 = QString("%1").arg(data7);
    tmp8 = QString("%1").arg(data8);
//    //往表格写数
//    insert_table(tempe1,tempe2,tempe3,tempe4,tempe5,tempe6,tempe7,tempe8);
    //实时存储
    QDateTime current_date_time =QDateTime::currentDateTime();   //获取当前时间
    QString str(current_date_time.toString("hh-mm-ss")+"  "+tmp1+"  "+tmp2
                +"  "+tmp3+"  "+tmp4+"  "+tmp5+"  "+tmp6+"  "+tmp7+"  "+tmp8+"\n");
    file.write(str.toStdString().c_str());
}

//从服务器读数据的读按钮槽方法
void UI_display::begin_collect()
{

    if (!modbusDevice)
        return;
//    ui->textEdit->clear();
    statusBar()->clearMessage();

    if (auto *reply = modbusDevice->sendReadRequest(readRequest(), ui->serverEdit->value())) {
        if (!reply->isFinished())
            connect(reply, &QModbusReply::finished, this, &UI_display::readReady);
        else
            delete reply; // broadcast replies return immediately
    } else {
        statusBar()->showMessage(tr("Read error: ") + modbusDevice->errorString(), 5000);
    }
//qDebug()<<"test0"<<endl;
}

void UI_display::on_pushButton_clicked()
{
    if (!modbusDevice)
        return;
    statusBar()->clearMessage();
    const QUrl url = QUrl::fromUserInput(ui->lineEdit->text());

    int iPort = url.port();
    QString qstrHost = url.host();

    modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
    modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.host());

    modbusDevice->setTimeout(ui->RespTime->value());
    modbusDevice->setNumberOfRetries(ui->NumOfRe->value());

    if (!modbusDevice->connectDevice())
    {
        QDateTime timecurrent1 = QDateTime::currentDateTime();
        QString time1 = timecurrent1.toString("[yy-MM-dd hh:mm:ss]");
        const QString RemindStr1 = time1+":"+tr("Connect failed ") + modbusDevice->errorString();
        ui->textEdit->setText(RemindStr1);
    }
    else
    {
        QDateTime timecurrent2 = QDateTime::currentDateTime();
        QString time2 = timecurrent2.toString("[yy-MM-dd hh:mm:ss]");
        const QString RemindStr2 =time2+":"+tr("Connect successed");
        ui->textEdit->setText(RemindStr2);
    }
}






void UI_display::on_pushButton_2_clicked()
{
    begin_collect();


}

void UI_display::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if (!reply)
        return;

    if (reply->error() == QModbusDevice::NoError)
    {
        const QModbusDataUnit unit = reply->result();


        qDebug()<<unit.values()<<endl;


        Uint16Tofloat tmp;
        tmp.arrUint16[0] = unit.value(0);
        tmp.arrUint16[1] = unit.value(1);


        float f= tmp.fValue;
        qDebug() << "readReady : " << f ;


        data1 = unit.value(0);
        data2 = unit.value(1);
        data3 = unit.value(2);
        data4 = unit.value(3);
        data5 = unit.value(4);
        data6 = unit.value(5);
        data7 = unit.value(6);
        data8 = unit.value(7);
        qDebug() << unit.value(0) << f ;
        qDebug() << unit.valueCount() << f ;
//        for (uint i = 0; i <20; i++)
//        {
//            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
//                                     .arg(QString::number(unit.value(i),
//                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
//            ui->Record->addItem(entry);
//        }
        for (uint i = 0; i < unit.valueCount(); i++)
        {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress())
                                     .arg(QString::number(unit.value(i),
                                          unit.registerType() <= QModbusDataUnit::Coils ? 10 : 16));
            QDateTime timecurrent5 = QDateTime::currentDateTime();
            QString time2 = timecurrent5.toString("[yy-MM-dd hh:mm:ss]");
            const QString RemindStr5 =time2+":"+entry;
            ui->textEdit->append(RemindStr5);

        }
    } else if (reply->error() == QModbusDevice::ProtocolError) {
        statusBar()->showMessage(tr("Read response error: %1 (Mobus exception: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->rawResult().exceptionCode(), -1, 16), 5000);
    } else {
        statusBar()->showMessage(tr("Read response error: %1 (code: 0x%2)").
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16), 5000);
    }

    reply->deleteLater();
}


QModbusDataUnit UI_display::readRequest() const
{
    //const auto table = "QModbusDataUnit::HoldingRegisters";
        //static_cast<QModbusDataUnit::RegisterType> ((QModbusDataUnit::HoldingRegisters).toInt());

    QModbusDataUnit::RegisterType table = QModbusDataUnit::HoldingRegisters;
    if (ui->lineEdit_3->text().isEmpty())
        ui->lineEdit_3->setText(QLatin1Literal("0"));

    int startAddress = (ui->lineEdit_3->text()).toInt();
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

//    // do not go beyond n entries
    int n = 10;

    if (ui->lineEdit_2->text().isEmpty())
        ui->lineEdit_2->setText(QLatin1Literal("5"));
//    int numberOfEntries = qMin(ui->readSize->currentText().toInt(), 20 - startAddress);
    int numberOfEntries =(ui->lineEdit_2->text()).toInt();
    return QModbusDataUnit(table, startAddress, numberOfEntries);

}

void UI_display::ShowInit()
{
    // 添加高频曲线
    ui->High_frequency->addGraph();         // 增加图层
    ui->High_frequency->graph(0)->setPen(QPen(Qt::red));
    ui->High_frequency->graph(0)->setAntialiasedFill(false);        // 设置图层反锯齿：关闭

    // 添加点
    ui->High_frequency->addGraph();
    ui->High_frequency->graph(1)->setPen(QPen(Qt::blue,3));       // 设置笔的颜色
    ui->High_frequency->graph(1)->setLineStyle(QCPGraph::lsNone);   // 不画线条
    ui->High_frequency->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);     // 设置点的形状

    //添加低频曲线
    ui->Low_frequency->addGraph();         // 增加图层
    ui->Low_frequency->graph(0)->setPen(QPen(Qt::red));
    ui->Low_frequency->graph(0)->setAntialiasedFill(false);        // 设置图层反锯齿：关闭

    // 添加点
    ui->Low_frequency->addGraph();
    ui->Low_frequency->graph(1)->setPen(QPen(Qt::blue,3));       // 设置笔的颜色
    ui->Low_frequency->graph(1)->setLineStyle(QCPGraph::lsNone);   // 不画线条
    ui->Low_frequency->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);     // 设置点的形状


    //坐标轴1设置
    // 设置x轴为时间
    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
  //  timeTicker->setTimeFormat("%h:%m:%s");
    //设置x轴有20个坐标，这个没用，要另一个策略函数配合
    //timeTicker->setTickCount(20);
    ui->High_frequency->xAxis->setTicker(timeTicker);
    ui->High_frequency->xAxis->setTickLabels(true);
//  QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
    //timeTicker->setDateTimeFormat("hh:mm:ss");             //另一种时间格式
    ui->High_frequency->axisRect()->setupFullAxesBox();     //坐标为矩
    // x轴设置
    ui->High_frequency->xAxis->setLabel("Time(s)");         // 设置横轴标签
    ui->High_frequency->yAxis->setRange(-10,70);       //设置纵轴范围
    ui->High_frequency->xAxis->setLabel("Time");
    ui->High_frequency->yAxis->setLabel("Data");
    ui->High_frequency->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);    //允许可缩放，移动
    ui->High_frequency->axisRect()->setRangeDrag(Qt::Horizontal);//设置曲线水平方向拖动
    //QList < QCPAxis *>  xis;
    //xis.append(ui->widget->xAxis);
    //ui->widget->axisRect()->setRangeZoomAxes(xis);

    // 轴随动
    connect(ui->High_frequency->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->High_frequency->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->High_frequency->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->High_frequency->yAxis2, SLOT(setRange(QCPRange)));




    //坐标轴2设置
    ui->Low_frequency->xAxis->setTicker(timeTicker);
    ui->Low_frequency->xAxis->setTickLabels(true);
//  QSharedPointer<QCPAxisTickerDateTime> timeTicker(new QCPAxisTickerDateTime);
//  timeTicker->setDateTimeFormat("hh:mm:ss");             //另一种时间格式
    ui->Low_frequency->axisRect()->setupFullAxesBox();     //坐标为矩
    // x轴设置
    ui->Low_frequency->xAxis->setLabel("Time(s)");         // 设置横轴标签
    ui->Low_frequency->yAxis->setRange(-10,70);       //设置纵轴范围
    ui->Low_frequency->xAxis->setLabel("Time");
    ui->Low_frequency->yAxis->setLabel("Data");
    ui->Low_frequency->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);    //允许可缩放，移动
    ui->Low_frequency->axisRect()->setRangeDrag(Qt::Horizontal);//设置曲线水平方向拖动
    //QList < QCPAxis *>  xis;
    //xis.append(ui->widget->xAxis);
    //ui->widget->axisRect()->setRangeZoomAxes(xis);

    // 轴随动
    connect(ui->Low_frequency->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Low_frequency->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->Low_frequency->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Low_frequency->yAxis2, SLOT(setRange(QCPRange)));
}







void UI_display::realtimedatashow()
{
    // calculate two new data points:
      double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;// time elapsed since start of demo, in seconds

      static double lastPointKey = 0;
      if (key-lastPointKey > 0.2) // at most add point every 2 ms
      {
        // add data to lines:
        ui->High_frequency->graph(0)->addData(key, data1);
        ui->High_frequency->graph(1)->addData(key, data1);
        ui->Low_frequency->graph(0)->addData(key, data2);
        ui->Low_frequency->graph(1)->addData(key, data2);
//        customPlot->graph(2)->addData(key, rand()%500);
//        customPlot->graph(3)->addData(key, rand()%400);
//        customPlot->graph(4)->addData(key, rand()%250);
//        customPlot->graph(5)->addData(key, rand()%100);
         lastPointKey = key;
      }



      ui->High_frequency->xAxis->setRange(key, 10, Qt::AlignRight);  //设置x轴的范围 显示个数为8个
      ui->High_frequency->replot();
      ui->Low_frequency->xAxis->setRange(key, 10, Qt::AlignRight);  //设置x轴的范围 显示个数为8个
      ui->Low_frequency->replot();

      // calculate frames per second:
      static double lastFpsKey;
      static int frameCount;
      ++frameCount;
      if (key-lastFpsKey > 2) // average fps over 2 seconds
      {
        lastFpsKey = key;
        frameCount = 0;
      }

}

void UI_display::on_start_clicked()
{

    _pTimerUpdate->start((ui->lineEdit_4->text()).toInt()); //1s更新一次
    qDebug() << "start" << endl;
    ui->pause->setEnabled(true);
    ui->start->setEnabled(false);
    // 操作记录
    QDateTime timecurrent4 = QDateTime::currentDateTime();
    QString time2 = timecurrent4.toString("[yy-MM-dd hh:mm:ss]");
    const QString RemindStr4 =time2+":"+tr("开始绘图");
    ui->textEdit->append(RemindStr4);
    // 创建文档
    QString thename = time2;
    fileName = thename+".txt";
    fileName = dir+"/"+fileName;
    file.setFileName(fileName);
    file.open(QFile::WriteOnly);
}



void UI_display::on_pause_clicked()
{
    _pTimerUpdate->stop();
    ui->pause->setEnabled(false);
    ui->start->setEnabled(true);
    // 操作记录
    QDateTime timecurrent5 = QDateTime::currentDateTime();
    QString time2 = timecurrent5.toString("[yy-MM-dd hh:mm:ss]");
    const QString RemindStr5 =time2+":"+tr("暂停绘图");
    ui->textEdit->append(RemindStr5);

    file.close();
}

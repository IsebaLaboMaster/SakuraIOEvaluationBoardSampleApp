#include "CSakuraIOEvaluationBoard.h"

CMPU9250Sample::CMPU9250Sample()
{
    //LCD初期化
    Lcd.cls();
    
    //MPU9250 初期化関連
    mpu9250.init(1, BITS_DLPF_CFG_188HZ);
    pc.printf("MPU9250 ok. WHOAMI=%02x\r\n", mpu9250.whoami());
    if (mpu9250.whoami() != 0x71) {
        pc.printf("[ERROR] MPU9250 init fail.\r\n");
    }
    
    //MPU9250データ格納用変数初期化
    Temperature = 0;
    for (int i = 0; i < 3; i++) {
        accelerometer_data[i] = 0;
        gyroscope_data[i] = 0;
        Magnetometer[i] =  0;
    }

    mpu9250.set_gyro_scale(BITS_FS_2000DPS);    //Set full scale range for gyros
    mpu9250.set_acc_scale(BITS_FS_16G);          //Set full scale range for accs
    mpu9250.calib_acc();
    mpu9250.AK8963_calib_Magnetometer();
    
    
    //LED、LCD表示モード初期化
    myLED1=0;
    myLED2=0;
    myLED3=0;
    myLED4=0;
    LcdLED=0;
    
    //sakura.ioポイント格納変数初期化
    sakuraioPoint=0;
    GetConnection=false;
}

CMPU9250Sample::~CMPU9250Sample()
{
}

void CMPU9250Sample::ReadMPU9250()
{
    mpu9250.read_all();
    Temperature = mpu9250.Temperature;
    for (int i = 0; i < 3; i++) {
        accelerometer_data[i] = mpu9250.accelerometer_data[i];
        gyroscope_data[i] = mpu9250.gyroscope_data[i];
        Magnetometer[i] =  mpu9250.Magnetometer[i];
    }
}

void CMPU9250Sample::PrintSirial()
{
    pc.printf("MPU9250\r\n");
    pc.printf("Temp: %.2fC\r\n", Temperature);
    
    pc.printf("accX: %.2f  accY: %.2f  accZ: %.2f\r\n", accelerometer_data[0],accelerometer_data[1],accelerometer_data[2]);
    pc.printf("gyroX: %.2f  gyroY: %.2f  gyroZ: %.2f\r\n", gyroscope_data[0],gyroscope_data[1],gyroscope_data[2]);
    pc.printf("magX: %.2f  magY: %.2f  magZ: %.2f\r\n", Magnetometer[0],Magnetometer[1],Magnetometer[2]);
}

void CMPU9250Sample::EnqueueSensorData()
{
    sakuraio.enqueueTx(0, sakuraioPoint);
    sakuraio.enqueueTx(1, Temperature);
    sakuraio.enqueueTx(2, accelerometer_data[0]);
    sakuraio.enqueueTx(3, accelerometer_data[1]);
    sakuraio.enqueueTx(4, accelerometer_data[2]);
    sakuraio.enqueueTx(5, gyroscope_data[0]);
    sakuraio.enqueueTx(6, gyroscope_data[1]);
    sakuraio.enqueueTx(7, gyroscope_data[2]);
    sakuraio.enqueueTx(8, Magnetometer[0]);
    sakuraio.enqueueTx(9, Magnetometer[1]);
    sakuraio.enqueueTx(10, Magnetometer[2]);
}

void CMPU9250Sample::SakuraIOSendData()
{
    //LCDちらつき防止のために、一旦結果だけをフラグに格納
    if( (sakuraio.getConnectionStatus() & 0x80) == 0x80 ) {
        GetConnection=true;
    } else {
        GetConnection=false;
    }
    
    //GetConnectionに値が代入されてからLCDクリアと描画実行
    Lcd.cls();
    if(GetConnection==true)
    {
        Lcd.printf("Online\n");
    }else
    {
        Lcd.printf("Offline\n");
    }
    if(mysw6==1) //SW6 7側ならデータ送信
    {
        sakuraioPoint++;
        Lcd.printf("ON %d", sakuraioPoint);
        EnqueueSensorData();
        sakuraio.send();
    }else
    {
        Lcd.printf("OFF %d", sakuraioPoint);
    }
}

void CMPU9250Sample::Run()
{
    if(mysw5==1) //SW5 7側へ
    {
        if(LcdLED!=1) LcdLED=1; //LCD_LED 点灯
    }else //SW5 6側へ
    {
        if(LcdLED!=0) LcdLED=0; //LCD_LED 消灯
    }

    ReadMPU9250();
    SakuraIOSendData();
    PrintSirial();
    
    wait(1);
}
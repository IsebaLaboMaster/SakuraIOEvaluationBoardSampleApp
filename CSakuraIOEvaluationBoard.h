#ifndef _CSAKURAIO_EVALUATION_BOARD_HEADER
#define _CSAKURAIO_EVALUATION_BOARD_HEADER

#include "mbed.h"
#include <SakuraIO.h>
#include <gps.h>
#include <AQM0802A.h>
#include <BME280.h>
#include <MPU9250.h>

//クラス宣言ここから(新しいヘッダファイルを追加した際は、以下にクラス宣言を追記)
class CSakuraIOEvaluationBoard;

class CSwLEDTest;
class CEscapeLED;
class CTimerSample;
class CBME280Sample;
class CMPU9250Sample;
class CGNSSXA1110Sample;
//クラス宣言ここまで

/*
////////////////////////////
//基底クラス
////////////////////////////
*/
class CSakuraIOEvaluationBoard
{
public:
    CSakuraIOEvaluationBoard();
    virtual ~CSakuraIOEvaluationBoard();
    
    //メインルーチン
    virtual void Run();
    
    SakuraIO_I2C sakuraio;
    
    I2C Internal_i2c;

    AQM0802A Lcd;
    Serial pc;
    
    // GPS
    Serial gps;
    DigitalOut gps_en;
    GPSDecoder gps_decoder;
    
    // GPS UART buffer
    char uart_buffer[128];
    int uart_buffer_index;
    
    //BME280
    BME280 bme280;
    
    //MPU9250
    SPI internal_mpu9250_spi;
    mpu9250_spi mpu9250;
    
    //Sw
    DigitalIn mysw1;
    DigitalIn mysw2;
    DigitalIn mysw3;
    DigitalIn mysw4;
    DigitalIn mysw5;
    DigitalIn mysw6;
    
    //LED
    DigitalOut myLED1;
    DigitalOut myLED2;
    DigitalOut myLED3;
    DigitalOut myLED4;
    DigitalOut LcdLED;
    
    int sakuraioPoint;//sakura.ioの使用ポイントを格納
    bool GetConnection;//通信モジュールの通信状態を格納
};

/*
////////////////////////////
//派生クラス
////////////////////////////
*/

//スイッチ＆LED確認用テストプログラム
//
//〇動作
//・SW1～SW4を押している間、LED1～LED4が点灯
//・SW5を7側にすると、LCDのLEDが点灯
//・SW6を7側にすると、LED1～LED4及びLCDのLEDが点灯
//
class CSwLEDTest : public CSakuraIOEvaluationBoard
{
public:
    CSwLEDTest();
    virtual ~CSwLEDTest();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new CSwLEDTest();}
};

//スイッチ、LED、LCD動作用サンプルプログラム
//「逃げるLED」
//〇概要
//・点灯しているLEDのSWを押すと、光が逃げます。
//
//〇動作
//・SW1～4はそれぞれLED1～LED4に対応
//・起動時はLED1～LED4のどれかが点灯
//・点灯しているLEDのSWを押下すると、光は押下されていないSWのLEDに移動
//・SW1～4が全て押されていると、LCD_LEDが点灯(光がLCD_LEDに逃げる)
//
class CEscapeLED : public CSakuraIOEvaluationBoard
{
public:
    CEscapeLED();
    virtual ~CEscapeLED();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new CEscapeLED();}
    
    int OtherThanNum();
    void SetLED();
    
    int NowLED;
    int BeforeMissCount;
    int MissCount;
    
    DigitalIn* pDigitalInSW[4];
    DigitalOut* pDigitalOutLED[5];
};

//スイッチ、LED、LCD動作用サンプルプログラム
//「24時間時計」
//〇概要
//・何の変哲もない、24H表示のデジタル時計です。
//・時刻をLCDディスプレイに表示します。
//
//〇動作
//
//・SW1→時間を00:00:00にリセットします。内部の時間計測用の変数(float timer_count)も0にします。
//・SW2→1時間加算します。
//・SW3→1分加算します。
//・SW4→1秒加算します。
//・SW5→7側に切り替えるとタイマー動作を停止します。
//・SW6→7側に切り替えるとLCDのLEDが点灯します。
//・LED1～4→"0.2秒ごとに点灯個所が1個ずつ増え、1秒経過すると全LEDが消灯"という動作を毎秒繰り返します。
//
class CTimerSample : public CSakuraIOEvaluationBoard
{
public:
    CTimerSample();
    virtual ~CTimerSample();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new CTimerSample();}
    
    void AddOneSecond();
    void AddOneMinute();
    void AddOneHour();
    
    int GetSecond();
    int GetMinute();
    int GetHour();
    
    void SWInput();
    void LEDOutput();
    void ResetTimer();
    
    int BeforeSecond;
    int Second;
    float timer_count;
    float AccuracyTime;//何秒毎に時間のカウントを行っていくか
    
    int BeforeSW1,NowSW1;
    int BeforeSW2,NowSW2;
    int BeforeSW3,NowSW3;
    int BeforeSW4,NowSW4;
    int NowSW5;
    int NowSW6;
    
    bool StopFlag;
    
    DigitalIn* pDigitalInSW[6];
    DigitalOut* pDigitalOutLED[5];
};

//BME280、LCD、SW、LED動作用サンプルプログラム
//「温湿度・気圧測定計」
//
//〇概要
//・BME280からの温度・相対湿度のデータを元に露点温度・絶対湿度・比エンタルピーを計算し、表示します。
//・BME280からの気圧データも表示します。
//・表示ホールド機能付きです。
//・LCDバックライト機能付きです。
//
//〇動作
//ボタン操作は以下の通りになります。
//SW1:温度[℃]・相対湿度[%]表示モード
//SW2:露点温度[℃DP]・絶対湿度[g/kg(DA)]表示モード
//SW3:気圧[hPa]表示モード
//SW4:比エンタルピー[kJ/kg]表示モード
//SW5:LCD点灯・データ送信スイッチ
//     (LCD側にスライドすると点灯し、データを送信します。基板上では1秒ごとにデータが更新されますが、送信は10秒に1回行います)                     
//SW6:表示ホールドスイッチ
//    (LCD側にスライドするとホールド機能をONにします。ONの間はLCD点灯の如何に関わらずデータの送信は行われません)
//
//※LCDにsakura.ioポイント使用数を表示させていないので、無用の浪費に気を付けて下さい
//※表示モードに合わせてLEDの点灯位置が変わります。
//※表示ホールド機能をOFFにしている状態では、各表示モードを切り替える際に
//  スイッチを約1秒程度長押しする必要があります。LCDの点灯も1秒程度遅れます。
//  (センサーデータの取得にwaitタイマーが入っているため)
//※表示ホールド時は各スイッチの動作遅れはありません。
//
//送信データは以下の型式
//0ch:温度
//1ch:相対湿度
//
class CBME280Sample : public CSakuraIOEvaluationBoard
{
public:
    CBME280Sample();
    virtual ~CBME280Sample();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new CBME280Sample();}
    
    float CalcSteamPartialPressure();
    float CalcAbsoluteHumidity();
    float CalcDewPoint();
    float CalcSpecificEnthalpy();
    
    void SwitchCheck();
    void GetAndCalcCensorData();
    void PrintLCD();
    void PrintLCDHold();
    
    //通信関係処理
    void EnqueueSensorData();
    void SakuraIOSendData();
    
    int NowMode;
    int BeforeMode;
    bool Hold;
    
    float Temp;//気温
    float RelativeHumidity;//相対湿度
    float AtmosphericPressure;//気圧
    float DewPoint;//露点温度
    float AbsoluteHumidity;//絶対湿度
    float SpecificEnthalpy;//比エンタルピー
    float SteamPartialPressure;//水蒸気分圧
    
    //通信間隔用変数
    int SendTimer;
};

//MPU9250、LCD、SW動作用サンプルプログラム
//『「MMD on WebGL」ライブラリを使用した、3Dモデル操作』
//
//〇概要
//・EVBボードのMPU-9250センサーのデータを使用して、「MMD on WebGL」上のモデルを動かします。
//
//〇動作
//ボタン操作は以下の通りになります。
//SW5:LCD点灯スイッチ(LCD側にスライドすると点灯します)
//SW6:データ通信スイッチ(LCD側にスライドするとsakura.ioポイントを消費して1秒に1回データを送信します)
//
//LCD1行目:通信モジュールがオンラインかどうか(Online:オンライン Offline:オフライン)
//LCD2行目:データ通信をONにしているかどうか(ON:データ通信ON OFF:データ通信OFF)と、使用したsakuraioポイント数表示
//
class CMPU9250Sample : public CSakuraIOEvaluationBoard
{
public:
    CMPU9250Sample();
    virtual ~CMPU9250Sample();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new CMPU9250Sample();}
    
    void ReadMPU9250();
    void PrintSirial();
    void EnqueueSensorData();
    void SakuraIOSendData();
    
    float Temperature;
    float accelerometer_data[3];
    float gyroscope_data[3];
    float Magnetometer[3];
};

//GNSSモジュール、LCD、SW動作用サンプルプログラム
//『GPS送受信機』
//
//〇概要
//・EVBボードのGNSSモジュールが受信した位置情報を通信モジュールを介し送信することができます。
//・Web側からの要請に応じてWebSocket通信にて位置情報を報告させることもできます。
//
//〇動作
//ボタン操作は以下の通りになります。
//SW1:通信モジュールの通信状態(1行目)と、データ送信状態・sakura.ioポイント使用量(2行目)を表示します。
//SW2:GNSSモジュールから取得した経度(1行目)、緯度(2行目)を表示します。
//SW4:押下している間、1秒ごとに「使用sakura.ioポイント数、経度、緯度、UNIX時間」を送信します。
//SW5:LCD点灯スイッチ(LCD側にスライドすると点灯します)
//SW6:自動データ通信スイッチ(LCD側にスライドするとsakura.ioポイントを消費して1秒に1回データを送信します)
//
class CGNSSXA1110Sample : public CSakuraIOEvaluationBoard
{
public:
    CGNSSXA1110Sample();
    virtual ~CGNSSXA1110Sample();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new CGNSSXA1110Sample();}

    void SwitchCheck();
    void PrintSirial();
    void EnqueueSensorData();
    void SakuraIOSendData();
    void SakuraIOReceptionData();
    void gps_uart_buffering_handler();

    uint32_t NowOrder;//Webからの値を格納
    int NowMode;
    int BeforeMode;
};

/////////////////////////
//ヘッダファイルひな形
/////////////////////////
//※ヘッダファイルを追加する場合は、このファイル内に追加するようにしてください。
//※.cppのヘッダファイルには「CSakuraIOEvaluationBoard.h」をインクルードしてください。
//※main.cppファイルのclass作成用関数テーブル格納配列にクラス作成関数(ClassName::New)を追記するのを忘れないでください。
/////////////////////////
//
/*
class ClassName(任意の名前) : public CSakuraIOEvaluationBoard
{
public:
    ClassName();
    virtual ~ClassName();
    virtual void Run();
    static CSakuraIOEvaluationBoard* New(){return new ClassName();}
};
*/

#endif
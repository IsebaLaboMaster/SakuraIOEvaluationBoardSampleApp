#include "CSakuraIOEvaluationBoard.h"

CBME280Sample::CBME280Sample()
{
    bme280.initialize();
    Lcd.cls();
    
    //各種パラメータ初期化
    Temp=0.0f;
    RelativeHumidity=0.0f;
    AtmosphericPressure=0.0f;
    SpecificEnthalpy=0.0f;
    AbsoluteHumidity=0.0f;
    DewPoint=0.0f;
    SteamPartialPressure=0.0f;
    
    //表示ホールド機能OFF
    Hold=false;
    
    //LED、LCD表示モード初期化
    myLED1=1;
    myLED2=0;
    myLED3=0;
    myLED4=0;
    BeforeMode=1;
    NowMode=1;
    
    SendTimer=0;
}

CBME280Sample::~CBME280Sample()
{
}

//BME280からのデータ取得と各種パラメータ
void CBME280Sample::GetAndCalcCensorData()
{
    //BME280センサーからデータを取得
    Temp=bme280.getTemperature();
    RelativeHumidity=bme280.getHumidity();
    AtmosphericPressure=bme280.getPressure();
    
    //取得したデータを元に以下のパラメータを計算
    SteamPartialPressure=CalcSteamPartialPressure();
    AbsoluteHumidity=CalcAbsoluteHumidity();
    SpecificEnthalpy=CalcSpecificEnthalpy();
    DewPoint=CalcDewPoint();
}

//水蒸気分圧計算
float CBME280Sample::CalcSteamPartialPressure()
{
    return 6.1078f*powf(10.0f,(7.5f*Temp)/(Temp+237.3f))*RelativeHumidity/100.0f;
}

//絶対湿度計算
float CBME280Sample::CalcAbsoluteHumidity()
{
    //※参考 大気圧:1013.25[hPa]
    return 0.62198f*SteamPartialPressure/(AtmosphericPressure-SteamPartialPressure);
}

//露点温度計算
float CBME280Sample::CalcDewPoint()
{
    return (237.3f*log10f(SteamPartialPressure/6.1078f))/(7.5f+log10f(6.1078f/SteamPartialPressure));
}

//比エンタルピー計算
float CBME280Sample::CalcSpecificEnthalpy()
{
    return 1.006f*Temp+(2501.0f+1.805f*Temp)*AbsoluteHumidity;
}

//スイッチ・LED処理
void CBME280Sample::SwitchCheck()
{
    BeforeMode=NowMode;
    if(mysw1==0) //SW1押下
    {
        myLED1=1; //LED1 点灯
        myLED2=0;
        myLED3=0;
        myLED4=0;
        NowMode=1;
    }
    
    if(mysw2==0) //SW2押下
    {
        myLED1=0;
        myLED2=1; //LED2 点灯
        myLED3=0;
        myLED4=0;
        NowMode=2;
    }
    
    if(mysw3==0) //SW3押下
    {
        myLED1=0;
        myLED2=0;
        myLED3=1; //LED3 点灯
        myLED4=0;
        NowMode=3;
    }
    
    if(mysw4==0) //SW4押下
    {
        myLED1=0;
        myLED2=0;
        myLED3=0;
        myLED4=1; //LED4 点灯
        NowMode=4;
    }
    
    if(mysw5==1) //SW5 7側へ
    {
        if(LcdLED!=1) LcdLED=1; //LCD_LED 点灯
    }else //SW5 6側へ
    {
        if(LcdLED!=0) LcdLED=0; //LCD_LED 消灯
    }
    
    if(mysw6==1) //SW6 7側へ
    {
        if(Hold!=true) Hold=true; //表示ホールド機能ON
    }else //SW6 6側へ
    {
        if(Hold!=false) Hold=false; //表示ホールド機能OFF
    }
}

//表示ホールド機能ON時のLCD表示処理
void CBME280Sample::PrintLCDHold()
{
    if(BeforeMode!=1 && NowMode==1)
    {
        Lcd.cls();
        Lcd.printf("%0.1fC\n%0.1f%%", Temp,RelativeHumidity);
    }
    
    if(BeforeMode!=2 && NowMode==2)
    {
        Lcd.cls();
        Lcd.printf("%0.1fCDP\n%0.1fg/kg", DewPoint,AbsoluteHumidity*1000.0f);
    }
    
    if(BeforeMode!=3 && NowMode==3)
    {
        Lcd.cls();
        Lcd.printf("%0.1f\n     hPa", AtmosphericPressure);
    }
    
    if(BeforeMode!=4 && NowMode==4)
    {
        Lcd.cls();
        Lcd.printf("%0.1f\n   kJ/kg", SpecificEnthalpy);
    }
}

void CBME280Sample::PrintLCD()
{
    Lcd.cls();
    switch(NowMode)
    {
    case 1://温度・相対湿度表示モード
        Lcd.cls();
        Lcd.printf("%0.1fC\n%0.1f%%", Temp,RelativeHumidity);
        break;
        
    case 2://露点温度・絶対湿度表示モード
        Lcd.printf("%0.1fCDP\n%0.1fg/kg", DewPoint,AbsoluteHumidity*1000.0f);
        break;
        
    case 3://気圧表示モード
        Lcd.printf("%0.1f\n     hPa", AtmosphericPressure);
        break;
        
    case 4://比エンタルピー表示モード
        Lcd.printf("%0.1f\n   kJ/kg", SpecificEnthalpy);
        break;
    }
    pc.printf("%0.1f[C] %0.1f[%%] %0.1f[CDP] %0.1f[g/kg(DA)] %0.1f[hPa] %0.1f[kJ/kg] %0.1f[hPa] \r\n", 
                            Temp,RelativeHumidity,DewPoint,AbsoluteHumidity*1000.0f,AtmosphericPressure,SpecificEnthalpy,SteamPartialPressure);
}

//通信関係関数ここから
void CBME280Sample::EnqueueSensorData()
{
    sakuraio.enqueueTx(0, Temp);
    sakuraio.enqueueTx(1, RelativeHumidity);
}

void CBME280Sample::SakuraIOSendData()
{
    EnqueueSensorData();
    sakuraio.send();//送信
}
//通信関係関数ここまで

void CBME280Sample::Run()
{
    SwitchCheck();
    if(Hold==false)
    {
        GetAndCalcCensorData();
        PrintLCD();
        
        //通信のための処理ここから
        SendTimer++;
        if(SendTimer>=10 && mysw5==1)//10秒以上経過かつSW5がLCD側(LCD点灯)ならデータを送信する
        {
            //通信モジュールによるデータ送信を無効化する場合は以下の「SakuraIOSendData();」の行をコメントする
            SakuraIOSendData();
            SendTimer=0;
        }
        //通信のための処理ここまで
        
        wait(1);
    }else PrintLCDHold();
}
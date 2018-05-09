#include "CSakuraIOEvaluationBoard.h"

//コンストラクタ
CTimerSample::CTimerSample()
{
    Lcd.cls();
    
    AccuracyTime=0.1f;//0.1秒毎に時間のカウントを行っていく
    StopFlag=false;
    
    timer_count=0.0f;//時間計測用変数
    BeforeSecond=0;
    Second=0;
    
    NowSW1=0;
    NowSW2=0;
    NowSW3=0;
    NowSW4=0;
    NowSW5=0;
    NowSW6=0;
    BeforeSW1=0;
    BeforeSW2=0;
    BeforeSW3=0;
    BeforeSW4=0;
    
    pDigitalInSW[0]=new DigitalIn(PC_14);//SW1
    pDigitalInSW[1]=new DigitalIn(PH_0);//SW2
    pDigitalInSW[2]=new DigitalIn(PC_0);//SW3
    pDigitalInSW[3]=new DigitalIn(PC_1);//SW4
    pDigitalInSW[4]=new DigitalIn(PC_2);//SW5
    pDigitalInSW[5]=new DigitalIn(PC_3);//SW6
    
    pDigitalOutLED[0]=new DigitalOut(PC_13);//LED1
    pDigitalOutLED[1]=new DigitalOut(PC_15);//LED2
    pDigitalOutLED[2]=new DigitalOut(PH_1);//LED3
    pDigitalOutLED[3]=new DigitalOut(PC_4);//LED4
    pDigitalOutLED[4]=new DigitalOut(PC_5);//LCD_LED
    
    //スイッチ状態格納変数初期化
    NowSW1=pDigitalInSW[0]->read();
    NowSW2=pDigitalInSW[1]->read();
    NowSW3=pDigitalInSW[2]->read();
    NowSW4=pDigitalInSW[3]->read();
    NowSW5=pDigitalInSW[4]->read();
    NowSW6=pDigitalInSW[5]->read();
    
    //LED初期化
    pDigitalOutLED[0]->write(0);//LED1消灯
    pDigitalOutLED[1]->write(0);//LED2消灯
    pDigitalOutLED[2]->write(0);//LED3消灯
    pDigitalOutLED[3]->write(0);//LED4消灯
    pDigitalOutLED[4]->write(0);//LCD_LED消灯
    
    Lcd.cls();
    Lcd.printf("%02d:%02d:%02d\n", GetHour(),GetMinute(),GetSecond());
}

//デストラクタ
CTimerSample::~CTimerSample()
{
    for(int i=0;i<4;i++)
    {
        delete pDigitalInSW[i];
        delete pDigitalOutLED[i];
        pDigitalInSW[i]=NULL;
        pDigitalOutLED[i]=NULL;
    }
}

//SW入力処理
void CTimerSample::SWInput()
{
    BeforeSW1=NowSW1;
    BeforeSW2=NowSW2;
    BeforeSW3=NowSW3;
    BeforeSW4=NowSW4;
    
    NowSW1=pDigitalInSW[0]->read();
    NowSW2=pDigitalInSW[1]->read();
    NowSW3=pDigitalInSW[2]->read();
    NowSW4=pDigitalInSW[3]->read();
    NowSW5=pDigitalInSW[4]->read();
    NowSW6=pDigitalInSW[5]->read();
    
    if(BeforeSW1==1 && NowSW1==0)
    {
        ResetTimer();
    }
    
    if(BeforeSW2==1 && NowSW2==0)
    {
        AddOneHour();
    }
    
    if(BeforeSW3==1 && NowSW3==0)
    {
        AddOneMinute();
    }
    
    if(BeforeSW4==1 && NowSW4==0)
    {
        AddOneSecond();
    }
    
    if(NowSW5==1)//SW5 7側へ
    {
        if(StopFlag!=true)
        {
            StopFlag=true;
        }
    }else //SW5 6側へ
    {
        if(StopFlag!=false)
        {
            StopFlag=false;
        }
    }
    
    if(NowSW6==1)//SW6 7側へ
    {
        if(pDigitalOutLED[4]->read()==0)//LCD_LEDが点灯していなかったら点灯
        {
            pDigitalOutLED[4]->write(1);
        }
    }else
    {
        if(pDigitalOutLED[4]->read()==1)//LCD_LEDが点灯していたら
        {
            pDigitalOutLED[4]->write(0);
        }
    }
}

//LED点灯処理
void CTimerSample::LEDOutput()
{
    if(timer_count<0.2f)
    {
        pDigitalOutLED[0]->write(0);//LED1消灯
        pDigitalOutLED[1]->write(0);//LED2消灯
        pDigitalOutLED[2]->write(0);//LED3消灯
        pDigitalOutLED[3]->write(0);//LED4消灯
    }else if(timer_count>=0.2f && timer_count<0.4f)
    {
        pDigitalOutLED[0]->write(0);//LED1消灯
        pDigitalOutLED[1]->write(0);//LED2消灯
        pDigitalOutLED[2]->write(0);//LED3消灯
        pDigitalOutLED[3]->write(1);//LED4点灯
    }else if(timer_count>=0.4f && timer_count<0.6f)
    {
        pDigitalOutLED[0]->write(0);//LED1消灯
        pDigitalOutLED[1]->write(0);//LED2消灯
        pDigitalOutLED[2]->write(1);//LED3点灯
        pDigitalOutLED[3]->write(1);//LED4点灯
    }else if(timer_count>=0.6f && timer_count<0.8f)
    {
        pDigitalOutLED[0]->write(0);//LED1消灯
        pDigitalOutLED[1]->write(1);//LED2点灯
        pDigitalOutLED[2]->write(1);//LED3点灯
        pDigitalOutLED[3]->write(1);//LED4点灯
    }else if(timer_count>=0.8f)
    {
        pDigitalOutLED[0]->write(1);//LED1点灯
        pDigitalOutLED[1]->write(1);//LED2点灯
        pDigitalOutLED[2]->write(1);//LED3点灯
        pDigitalOutLED[3]->write(1);//LED4点灯
    }
}

//時刻及び時刻計測用タイマ初期化。LED初期化
void CTimerSample::ResetTimer()
{
    timer_count=0.0f;
    BeforeSecond=0;
    Second=0;
    
    pDigitalOutLED[0]->write(0);//LED1消灯
    pDigitalOutLED[1]->write(0);//LED2消灯
    pDigitalOutLED[2]->write(0);//LED3消灯
    pDigitalOutLED[3]->write(0);//LED4消灯
}

//1秒加算
void CTimerSample::AddOneSecond()
{
    Second=Second+1;
    if(Second/(24*60*60)>=1)
    {
        Second=Second%(24*60*60);
    }
}

//1分加算
void CTimerSample::AddOneMinute()
{
    Second=Second+60;
    if(Second/(24*60*60)>=1)
    {
        Second=Second%(24*60*60);
    }
}

//1時間加算
void CTimerSample::AddOneHour()
{
    Second=Second+3600;
    if(Second/(24*60*60)>=1)
    {
        Second=Second%(24*60*60);
    }
}

//現在の秒数を得る
int CTimerSample::GetSecond()
{
    return Second%60;
}

//現在の分を得る
int CTimerSample::GetMinute()
{
    return (Second/60)%60;
}

//現在の時間を得る
int CTimerSample::GetHour()
{
    return (Second/3600)%24;
}

//実行関数 
void CTimerSample::Run()
{
    BeforeSecond=Second;
    SWInput();
    LEDOutput();
    
    wait(AccuracyTime);
    if(StopFlag==false)
    {
        timer_count=timer_count+AccuracyTime;
        if(timer_count>=1.0f)
        {
            AddOneSecond();
            timer_count=0.0f;
        }
    }
    
    //秒数が変化したとき、またはSW1が押された時にLCDディスプレイを更新
    if((BeforeSecond !=Second) || (BeforeSW1==1 && NowSW1==0))
    {
        Lcd.cls();
        Lcd.printf("%02d:%02d:%02d\n", GetHour(),GetMinute(),GetSecond());
        pc.printf("%02d:%02d:%02d %d\r\n", GetHour(),GetMinute(),GetSecond(),Second);
    }
}
#include "CSakuraIOEvaluationBoard.h"

CEscapeLED::CEscapeLED()
{
    Lcd.cls();
    
    pDigitalInSW[0]=new DigitalIn(PC_14);//SW1
    pDigitalInSW[1]=new DigitalIn(PH_0);//SW2
    pDigitalInSW[2]=new DigitalIn(PC_0);//SW3
    pDigitalInSW[3]=new DigitalIn(PC_1);//SW4
    
    pDigitalOutLED[0]=new DigitalOut(PC_13);//LED1
    pDigitalOutLED[1]=new DigitalOut(PC_15);//LED2
    pDigitalOutLED[2]=new DigitalOut(PH_1);//LED3
    pDigitalOutLED[3]=new DigitalOut(PC_4);//LED4
    pDigitalOutLED[4]=new DigitalOut(PC_5);//LCD_LED
    
    pDigitalOutLED[0]->write(0);//LED1消灯
    pDigitalOutLED[1]->write(0);//LED2消灯
    pDigitalOutLED[2]->write(0);//LED3消灯
    pDigitalOutLED[3]->write(0);//LED4消灯
    pDigitalOutLED[4]->write(0);//LCD_LED
    
    BeforeMissCount=0;
    MissCount=0;//カウント
    NowLED=rand()%4;//初期LED点灯位置設定
    SetLED();
}

CEscapeLED::~CEscapeLED()
{
    for(int i=0;i<4;i++)
    {
        delete pDigitalInSW[i];
        pDigitalInSW[i]=NULL;
    }
    
    for(int i=0;i<5;i++)
    {
        delete pDigitalOutLED[i];
        pDigitalOutLED[i]=NULL;
    }
}

int CEscapeLED::OtherThanNum()
{
    //SW1～4まで全て押されていたらLCD_LEDを点灯させる
    if(pDigitalInSW[0]->read()==0 && pDigitalInSW[1]->read()==0 && pDigitalInSW[2]->read()==0 && pDigitalInSW[3]->read()==0)
    {
        return 4;    
    }
    
    int buf=0;
    while(1)
    {
        buf=rand()%4;
        if(pDigitalInSW[buf]->read()==1 && pDigitalOutLED[buf]->read()==0)//押されていない&点灯していない場所だったら
        {
            return buf;
        }
    }
}

void CEscapeLED::SetLED()
{
    pDigitalOutLED[NowLED]->write(1);
}

void CEscapeLED::Run()
{
    BeforeMissCount=MissCount;
    SetLED();
    
    //点灯LEDのSWが押されたら
    if(NowLED<4)
    {
        if(pDigitalInSW[NowLED]->read()==0 && pDigitalOutLED[NowLED]->read()==1) 
        {
            MissCount++;
            if(MissCount>9999) MissCount=0;//10000以上カウントしたらカウンターリセット
            pDigitalOutLED[NowLED]->write(0); //今のLEDを消灯
            NowLED=OtherThanNum(); //スイッチが押されていないLED不点灯個所を探す
        }
    }else//全てのSWが押されていたら
    {
        NowLED=OtherThanNum();
        if(NowLED!=4) pDigitalOutLED[4]->write(0); //LCD_LEDを消灯
    }
    
    //LCD関係処理
    if(BeforeMissCount != MissCount)//カウント値に変化があった時のみ以下の処理
    {
        Lcd.cls();
        Lcd.printf("Miss%d\n", MissCount);
        if(NowLED==4)
        {
            Lcd.printf("ESCAPE!\n");
        }
        pc.printf("NowLED=%d MissCount=%d\r\n", NowLED,MissCount);
    }
}
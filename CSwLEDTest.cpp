#include "CSakuraIOEvaluationBoard.h"

CSwLEDTest::CSwLEDTest()
{
    myLED1=1;
    myLED2=0;
    myLED3=0;
    myLED4=0;
    LcdLED=0;
}
    
CSwLEDTest::~CSwLEDTest()
{
}
    
void CSwLEDTest::Run()
{
    if(mysw1==0) //SW1押下
    {
        myLED1=1; //LED1 点灯
    }else //SW1離す
    {
        myLED1=0; //LED1 消灯
    }
    
    if(mysw2==0) //SW2押下
    {
        myLED2=1; //LED2 点灯
    }else //SW2離す
    {
        myLED2=0; //LED2 消灯
    }
    
    if(mysw3==0) //SW3押下
    {
        myLED3=1; //LED3 点灯
    }else //SW3離す
    {
        myLED3=0; //LED3 消灯
    }
    
    if(mysw4==0) //SW4押下
    {
        myLED4=1; //LED4 点灯
    }else //SW4離す
    {
        myLED4=0; //LED4 消灯
    }
    
    if(mysw5==1) //SW5 7側へ
    {
        LcdLED=1; //LCD_LED 点灯
    }else //SW5 6側へ
    {
        LcdLED=0; //LCD_LED 消灯
    }
    
    if(mysw6==1) //SW6 7側へ
    {
        myLED1=1; //LED1 点灯
        myLED2=1; //LED2 点灯
        myLED3=1; //LED3 点灯
        myLED4=1; //LED4 点灯
        LcdLED=1; //LCD_LED 点灯
    }
}
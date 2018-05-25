#include <mbed.h>
#include "CSakuraIOEvaluationBoard.h"

//
//2018年2月19日(月)
//sakura.Evaluation Board サンプルプログラム集
//
//操作方法
//1.「STM32Reset」ボタンを押し、基板をリセットすると、アプリケーション選択画面になる。
//2.SW1でカウントアップ、SW2でカウントダウンするので、お好みのアプリケーションを選択する。
//3.SW4押下で決定。選択されたアプリケーションが実行される。
//4.別のアプリケーションを動作させたい場合は1.に戻る。
//
//※SW6はLCDのバックライト点灯スイッチです。(6側でOFF、7側(LCD側)で点灯)
//
//初期アプリケーションナンバー
//1.スイッチ＆LED確認用テストプログラム
//2.「逃げるLED」
//3.「24時間時計」
//4.「温湿度・気圧測定計」
//5.『「MMD on WebGL」ライブラリを使用した、3Dモデル操作』
//6.「GPS送受信機」
//

typedef CSakuraIOEvaluationBoard* (*SelectProgram)();

int main() {
    CSakuraIOEvaluationBoard* pSakuraIOEvaluationBoard;
    pSakuraIOEvaluationBoard=NULL;
    
    // Internal I2C
    I2C internal_i2c(PB_9, PB_8);
    AQM0802A Lcd(internal_i2c);
    
    //Sw
    DigitalIn mysw1(PC_14);
    DigitalIn mysw2(PH_0);
    DigitalIn mysw4(PC_1);
    DigitalIn mysw6(PC_3);
    
    //LED
    DigitalOut LcdLED(PC_5);
    
    //スイッチ動作用フラグ
    bool mysw1flag=false;
    bool beforemysw1flag=false;
    bool mysw2flag=false;
    bool beforemysw2flag=false;

    //class作成用関数テーブル
    //新しいclassを作成してアプリケーション選択画面に追加する場合は以下の配列内にclass作成用関数を追加する
    SelectProgram SelectProgramArray[]=
    {
        CSwLEDTest::New,
        CEscapeLED::New,
        CTimerSample::New,
        CBME280Sample::New,
        CMPU9250Sample::New,
        CGNSSXA1110Sample::New,
        CSakuraIOReversi::New
    };
    
    int NowSelectProgram=1;
    const int ArraySize=sizeof(SelectProgramArray)/sizeof(SelectProgramArray[0]);
    
    Lcd.cls();
    Lcd.printf("App No.\n");
    Lcd.printf("%d",NowSelectProgram);
    
    //アプリケーション選択用ルーチン
    while(1)
    {
        beforemysw1flag=mysw1flag;
        beforemysw2flag=mysw2flag;
        
        if(mysw1==0) //SW1押下
        {
            mysw1flag=true;
        }else mysw1flag=false;
        
        if(mysw2==0) //SW2押下
        {
            mysw2flag=true;
        }else mysw2flag=false;
        
        if(mysw4==0) //SW4押下
        {
            Lcd.cls();
            Lcd.printf("OK\n");
            break;
        }
        
        if(mysw6==1) //SW6 7側へ
        {
            if(LcdLED!=1) LcdLED=1; //LCD_LED 点灯
        }else //SW6 6側へ
        {
            if(LcdLED!=0) LcdLED=0; //LCD_LED 消灯
        }
        
        if(beforemysw1flag==false && mysw1flag==true)
        {
            NowSelectProgram++;
            if(NowSelectProgram>ArraySize) NowSelectProgram=1;
            Lcd.cls();
            Lcd.printf("App No.\n");
            Lcd.printf("%d",NowSelectProgram);
        }
        if(beforemysw2flag==false && mysw2flag==true)
        {
            NowSelectProgram--;
            if(NowSelectProgram<1) NowSelectProgram=ArraySize;
            Lcd.cls();
            Lcd.printf("App No.\n");
            Lcd.printf("%d",NowSelectProgram);
        }
        wait(0.01f);
    }
    
    //NowSelectProgramに応じたオブジェクト作成し、そのポインタをpSakuraIOEvaluationBoardに格納
    pSakuraIOEvaluationBoard=SelectProgramArray[NowSelectProgram-1]();
    
    //メインループ
    while(1)
    {
        pSakuraIOEvaluationBoard->Run();
    }
}

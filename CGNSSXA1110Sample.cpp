#include "CSakuraIOEvaluationBoard.h"

CGNSSXA1110Sample::CGNSSXA1110Sample()
{
    //LCD初期化
    Lcd.cls();
    pc.printf("CGNSSXA1110Sample ok. \r\n");
    
    //LED、LCD表示モード初期化
    myLED1=1;
    myLED2=0;
    myLED3=0;
    myLED4=0;
    LcdLED=0;
    
    //sakura.ioポイント格納変数初期化
    sakuraioPoint=0;
    GetConnection=false;
    
    BeforeMode=1;
    NowMode=1;
    
    //char配列0クリア
    memset( uart_buffer, 0, sizeof( uart_buffer ) );
    
    // active high
    gps_en = 1;
    gps.attach(this, &CGNSSXA1110Sample::gps_uart_buffering_handler, Serial::RxIrq);
}

CGNSSXA1110Sample::~CGNSSXA1110Sample()
{
}

void CGNSSXA1110Sample::SwitchCheck()
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
    
    if(mysw4==0) //SW4押下
    {
        myLED4=1;
    }else myLED4=0;
    
    if(mysw5==1) //SW5 7側へ
    {
        if(LcdLED!=1) LcdLED=1; //LCD_LED 点灯
    }else //SW5 6側へ
    {
        if(LcdLED!=0) LcdLED=0; //LCD_LED 消灯
    }
}

void CGNSSXA1110Sample::gps_uart_buffering_handler()
{
    while (gps.readable() == 1) {
        char c = gps.getc();
        if (c == '\r') {
            continue;
        }
        uart_buffer[uart_buffer_index] = c;
        uart_buffer_index++;
        if (c == '\n') {
            uart_buffer[uart_buffer_index - 1] = '\0';
            gps_decoder.set_nmea_message(uart_buffer);
            gps_decoder.decode();
            uart_buffer_index = 0;
        }
    }
}

void CGNSSXA1110Sample::EnqueueSensorData()
{
    sakuraio.enqueueTx(0, sakuraioPoint);
    sakuraio.enqueueTx(1, gps_decoder.get_longitude());
    sakuraio.enqueueTx(2, gps_decoder.get_latitude());
    sakuraio.enqueueTx(3, gps_decoder.get_unixtime());
}

void CGNSSXA1110Sample::SakuraIOReceptionData()
{
    // 受信キューの状態を取得
    uint8_t rxAvailable;
    uint8_t rxQueued;
    sakuraio.getRxQueueLength(&rxAvailable, &rxQueued);
      
    // 受信キューにたまっているメッセージの数だけ繰り返す
    for (uint8_t i = 0; i < rxQueued; i++)
    {
        uint8_t channel;
        uint8_t type;
        uint8_t data[8];
        int64_t offset;
        NowOrder=0;
        
        //キューからのメッセージを取り出し
        uint8_t ret = sakuraio.dequeueRx(&channel, &type, data, &offset);
        
        //成功したら以下を実行
        if (ret == CMD_ERROR_NONE) {
            sakuraioPoint++;
              
            pc.printf("channel:%d type:%c ",channel,type);
        
            //バイト列から変換して値をプリント
            if (type == 'i') {
            memcpy(&NowOrder, &data, sizeof(uint32_t));
            pc.printf("value:%d\r\n",NowOrder);
            }
        }else{
            pc.printf("ERROR\r\n");
        }
    }
}

void CGNSSXA1110Sample::SakuraIOSendData()
{
    //LCDちらつき防止のために、一旦結果だけをフラグに格納
    if( (sakuraio.getConnectionStatus() & 0x80) == 0x80 ) {
        GetConnection=true;
    } else {
        GetConnection=false;
    }
    
    //GetConnectionに値が代入されてからLCDクリアと描画実行
    Lcd.cls();
    switch(NowMode)
    {
    case 1:
        if(GetConnection==true)
        {
            Lcd.printf("Online\n");
        }else
        {
            Lcd.printf("Offline\n");
        }
        break;
        
    case 2:
        Lcd.printf("%f\n",gps_decoder.get_longitude());
        Lcd.printf("%f\n",gps_decoder.get_latitude());
        break;
    }
    
    if(mysw4==0 || mysw6==1 || NowOrder==1) //SW4押下,mysw6がLCD側、またはWebから(「0chにint型の1」を受信したら)指令があったら
    {
        NowOrder=0;
        sakuraioPoint++;//使用ポイントを1足す
        if(NowMode==1) Lcd.printf("ON %d", sakuraioPoint);
        EnqueueSensorData();
        sakuraio.send();//送信
    }else
    {
        if(NowMode==1) Lcd.printf("OFF %d", sakuraioPoint);
    }
}

void CGNSSXA1110Sample::PrintSirial()
{
    pc.printf("GPS\r\n");
    pc.printf("\tlat: %f%c\r\n", 
            gps_decoder.get_latitude(), 
            gps_decoder.get_latitude() >= 0 ? 'N' : 'S');
    pc.printf("\tlon: %f%c\r\n", 
            gps_decoder.get_longitude(),
            gps_decoder.get_longitude() >= 0 ? 'E' : 'W');
    pc.printf("\tspeed: %fkm/h\r\n", gps_decoder.get_speed());
    pc.printf("\tmove_direction: %f\r\n", gps_decoder.get_move_direction());
    pc.printf("\tdate: %d/%02d/%02d %02d:%02d:%02d (UTC)\r\n",
              gps_decoder.get_year(), gps_decoder.get_month(), gps_decoder.get_day(),
              gps_decoder.get_hour(), gps_decoder.get_min(), gps_decoder.get_sec());
    pc.printf("\tUNIX time: %d\r\n", gps_decoder.get_unixtime());
}

void CGNSSXA1110Sample::Run()
{
    SwitchCheck();
    SakuraIOReceptionData();
    SakuraIOSendData();
    PrintSirial();
    wait(1);
}
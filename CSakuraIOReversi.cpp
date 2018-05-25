#include "CSakuraIOEvaluationBoard.h"

//石色定義
#define BLACK_PIECE 1
#define WHITE_PIECE 2

CSakuraIOReversi::CSakuraIOReversi()
{
    debug=false;//PCとシリアル通信する際はこれっをtrueにする
    
    Lcd.cls();//LCDディスプレイクリア
    InitPieceAry();//石初期配置
    SetMyPiece(BLACK_PIECE);//自分の石色 　1:黒    2:白
    
    MyAttackX=1;//自分の手番のX座標
    MyAttackY=1;//自分の手番のY座標

    //LED、LCD表示モード初期化
    myLED1=0;
    myLED2=0;
    myLED3=0;
    myLED4=0;
    LcdLED=0;
    
    //スイッチ処理用変数初期化
    NowSW1=0;
    NowSW2=0;
    NowSW3=0;
    NowSW4=0;
    NowSW5=0;
    BeforeSW1=0;
    BeforeSW2=0;
    BeforeSW3=0;
    BeforeSW4=0;
    
    //受信データ格納用配列初期化
    for(int i=0;i<16;i++)
    {
        ReceiveData[i]=0;
    }
    
    PieceAryChangeSendData();//盤情報を送信データに変換
    EnqueueData(1);
    SakuraIOSendData();
}

CSakuraIOReversi::~CSakuraIOReversi()
{
}

//ゲームを初期状態にする。ただし、sakuraioPointは初期化しない
void CSakuraIOReversi::GameReset()
{
    Lcd.cls();//LCDディスプレイクリア
    InitPieceAry();//石初期配置
    SetMyPiece(BLACK_PIECE);//自分の石色 　1:黒    2:白
    
    MyAttackX=1;//自分の手番のX座標
    MyAttackY=1;//自分の手番のY座標

    //LED、LCD表示モード初期化
    myLED1=0;
    myLED2=0;
    myLED3=0;
    myLED4=0;
    LcdLED=0;
    
    //スイッチ処理用変数初期化
    NowSW1=0;
    NowSW2=0;
    NowSW3=0;
    NowSW4=0;
    NowSW5=0;
    BeforeSW1=0;
    BeforeSW2=0;
    BeforeSW3=0;
    BeforeSW4=0;
    
    //受信データ格納用配列初期化
    for(int i=0;i<16;i++)
    {
        ReceiveData[i]=0;
    }
}

int CSakuraIOReversi::MaxReturn(int num1,int num2)
{
    if(num1>num2)
    {
        return num1;
    }else return num2;
}

int CSakuraIOReversi::MinReturn(int num1,int num2)
{
    if(num1<num2)
    {
        return num1;
    }else return num2;
}

void CSakuraIOReversi::SetMyPiece(int mypiece)
{
    int buf=mypiece;
    if(buf<BLACK_PIECE)
    {
        buf=BLACK_PIECE;
    }else if(buf>WHITE_PIECE)
    {
        buf=WHITE_PIECE;
    }
    
    if(buf==BLACK_PIECE)//自分が黒番
    {
        MyPiece=BLACK_PIECE;
        YourPiece=WHITE_PIECE;
        MyTurn=true;
    }else//相手が黒番
    {
        MyPiece=WHITE_PIECE;
        YourPiece=BLACK_PIECE;
        MyTurn=false;
    }
}

void CSakuraIOReversi::InitPieceAry()
{
    //0:空白     1:黒    2:白
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            PieceAry[y][x]=0;
        }
    }
    
    //石初期配置
    PieceAry[3][3]=WHITE_PIECE;
    PieceAry[4][4]=WHITE_PIECE;
    PieceAry[3][4]=BLACK_PIECE;
    PieceAry[4][3]=BLACK_PIECE;
    
    //送信データも初期化
    PieceAryChangeSendData();
}

void CSakuraIOReversi::PieceAryChangeSendData()
{
    for(int y=0;y<8;y++)
    {
        PieceSendData[y]=10000000*PieceAry[y][0]+
                         1000000*PieceAry[y][1]+
                         100000*PieceAry[y][2]+
                         10000*PieceAry[y][3]+
                         1000*PieceAry[y][4]+
                         100*PieceAry[y][5]+
                         10*PieceAry[y][6]+
                         1*PieceAry[y][7];
    }
}

void CSakuraIOReversi::EnqueueData(int32_t order)
{
    int32_t TurnNum;
    
    if(MyTurn==true)
    {
        TurnNum=1;//今は私の手番
    }else TurnNum=2;//今は相手の手番
    
    sakuraio.enqueueTx(0, PieceSendData[0]);
    sakuraio.enqueueTx(1, PieceSendData[1]);
    sakuraio.enqueueTx(2, PieceSendData[2]);
    sakuraio.enqueueTx(3, PieceSendData[3]);
    sakuraio.enqueueTx(4, PieceSendData[4]);
    sakuraio.enqueueTx(5, PieceSendData[5]);
    sakuraio.enqueueTx(6, PieceSendData[6]);
    sakuraio.enqueueTx(7, PieceSendData[7]);

    sakuraio.enqueueTx(8, TurnNum);//現在の私の手番(1)か相手の手番(2)かを返す
    sakuraio.enqueueTx(9, (int32_t)MyPiece);//自分の石色を返す //自分の石色 　1:黒    2:白
    sakuraio.enqueueTx(10, order);
    
    //order=1:現在の盤情報を返す
    //order=2:ゲームリセット信号
    //order=3:ゲーム終了信号
}

void CSakuraIOReversi::SakuraIOSendData()
{
    //LCDちらつき防止のために、一旦結果だけをフラグに格納
    if( (sakuraio.getConnectionStatus() & 0x80) == 0x80 ) {
        GetConnection=true;
    } else {
        GetConnection=false;
    }
    
    sakuraioPoint++;//使用ポイントを1足す
    sakuraio.send();//送信
}

void CSakuraIOReversi::SakuraIOReceptionData()
{
    /*
    受信時
    0chの格納データ
      1:Web側接続後の盤情報確認
      2:Web側からの攻め手情報 このときch1に2桁の整数(int32_t)を格納(10の位:X 1の位:Y)
    */             
    //データ格納配列初期化
    for(int i=0;i<16;i++)
    {
        ReceiveData[i]=0;
    }
    
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
        
        //キューからのメッセージを取り出し
        uint8_t ret = sakuraio.dequeueRx(&channel, &type, data, &offset);
        
        //成功したら以下を実行
        if (ret == CMD_ERROR_NONE) {
            pc.printf("channel:%d type:%c ",channel,type);
            //バイト列から変換して値をプリント
            if (type == 'i') {
            memcpy(&ReceiveData[i], &data, sizeof(int32_t));
            pc.printf("%d:%d \r\n",i,ReceiveData[i]);
            }
        }else{
            pc.printf("ERROR\r\n");
        }
    }
    
    //受信したデータの処理は以下に記述
    //Web側接続後の盤情報確認があったら、現在の盤情報・手番・石色を返す
    
    switch(ReceiveData[0])
    {
    case 1://Webからの盤情報要求を受信したら
        sakuraioPoint++;//1回受信したことになるのでsakuraioPointに1加える
        PieceAryChangeSendData();//盤情報を送信データに変換
        if(EndGameJudge()==false)//まだゲームが続行可能な状態なら
        {
            EnqueueData(1);
        }else//ゲーム続行が不可能だったら
        {
            EnqueueData(3);
        }
        SakuraIOSendData();
        break;
    
    case 2://Webからの相手手番情報を受信したら
        if(MyTurn==true)//自分の手番の時
        {
            //自分の手番の時、相手が手番データを送ってきたとしても、何もしない。
        }else//相手番時に相手の攻め手が送信(1chが2)されてきたら、その手番が有効かどうかチェックする
        {
            sakuraioPoint++;//1回受信したことになるのでsakuraioPointに1加える
            int x=ReceiveData[1]/10%10;//2chは相手手番データ
            int y=ReceiveData[1]%10;
            if(AttackJudge(x,y,YourPiece,false)==true)
            {
                MyTurn=true;//相手の手が有効手だったら次は自分の手番にする
                if(MyAttackExist()==false)//但し、自分が打てる手が無かったら手番は相手に返す
                {
                    MyTurn=false;
                }
                PieceAryChangeSendData();//盤情報を送信データに変換
                
                if(EndGameJudge()==false)//まだゲームが続行可能な状態なら
                {
                    EnqueueData(1);
                }else//ゲーム続行が不可能だったら
                {
                    EnqueueData(3);
                }
                SakuraIOSendData();
            }
        }
        break;
    
    default:
        break;
    }
}

//ゲームが終了しているかどうか判定 終了ならtrueを返す。ゲーム続行可能ならfalseを返す
bool CSakuraIOReversi::EndGameJudge()
{
    int count=0;
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            if(PieceAry[y][x]==0)//空マスの数を数える。最終的に空マスが0ならゲーム終了
            {
                count++;
            }
        }
    }
    
    if(count==0)//空マスが全くない場合ゲームは終了
    {
        return true;
    }else
    {
        if(MyAttackExist()==false && YourAttackExist()==false)//自分も相手も置ける場所が無かったらゲーム終了
        {
            return true;
        }else return false;
    }
}
//自分の石を置ける場所があるかどうか調べる
bool CSakuraIOReversi::MyAttackExist()
{
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            if(PieceAry[y][x]==0)
            {
                if(AttackJudge(x,y,MyPiece,true)==true)
                {
                    return true;//一ヶ所でも置ける場所があったらtrueを返す
                }
            }
        }
    }
    return false;//ここまで来ると置ける場所がないということ。falseを返す。
}

//相手の石を置ける場所があるかどうか調べる
bool CSakuraIOReversi::YourAttackExist()
{
    for(int y=0;y<8;y++)
    {
        for(int x=0;x<8;x++)
        {
            if(PieceAry[y][x]==0)
            {
                if(AttackJudge(x,y,YourPiece,true)==true)
                {
                    return true;//一ヶ所でも置ける場所があったらtrueを返す
                }
            }
        }
    }
    return false;//ここまで来ると置ける場所がないということ。falseを返す。
}

void CSakuraIOReversi::CountUpAttackX()
{
    MyAttackX++;
    if(MyAttackX>9) MyAttackX=1;
}

void CSakuraIOReversi::CountUpAttackY()
{
    MyAttackY++;
    if(MyAttackY>9) MyAttackY=1;
}

void CSakuraIOReversi::SwitchCheck()
{
    BeforeSW1=NowSW1;
    BeforeSW2=NowSW2;
    BeforeSW3=NowSW3;
    BeforeSW4=NowSW4;
    
    if(mysw1==0) //SW1押下
    {
        NowSW1=0;
        myLED1=1; //LED1 点灯
    }else //SW1離す
    {
        NowSW1=1;
        myLED1=0; //LED1 消灯
    }
    if(BeforeSW1==1 && NowSW1==0) CountUpAttackX();
    
    if(mysw2==0) //SW2押下
    {
        NowSW2=0;
        myLED2=1; //LED2 点灯
    }else //SW2離す
    {
        NowSW2=1;
        myLED2=0; //LED2 消灯
    }
    if(BeforeSW2==1 && NowSW2==0) CountUpAttackY();
    
    if(mysw3==0) //SW3押下
    {
        NowSW3=0;
        myLED3=1; //LED3 点灯
    }else //SW3離す
    {
        NowSW3=1;
        myLED3=0; //LED3 消灯
    }
    
    //自分の手番送信処理
    if(BeforeSW3==1 && NowSW3==0 && MyTurn==true) //自分の手番でSW3を押下したら
    {
        int buf=MyAttackX*10+MyAttackY;
        if(buf<=88) //11～88ならば盤座標情報
        {
            if(AttackJudge(MyAttackX-1,MyAttackY-1,MyPiece,false)==true)//石が置けたなら
            {
                MyAttackX=1;
                MyAttackY=1;
                MyTurn=false;//手番を相手にする
                if(YourAttackExist()==false)//相手が置けない状態だったら相手の手番はパス
                {
                    MyTurn=true;
                }
                PieceAryChangeSendData();//盤情報を送信データに変換
                if(EndGameJudge()==false)//まだゲームが続行可能な状態なら
                {
                    EnqueueData(1);
                }else//ゲーム続行が不可能だったら
                {
                    EnqueueData(3);
                }
                SakuraIOSendData();
            }
        }
    }
    
    if(mysw4==0) //SW4押下
    {
        NowSW4=0;
        myLED4=1; //LED4 点灯
    }else //SW4離す
    {
        NowSW4=1;
        myLED4=0; //LED4 消灯
    }
    if(BeforeSW4==1 && NowSW4==0) //設定ボタン
    {
        int buf=MyAttackX*10+MyAttackY;
        if(buf>88) //11～77ならば盤座標情報
        {
            if(buf==98) //ゲームリセットし、私の手番を白にする。
            {
                GameReset();
                PieceAryChangeSendData();//盤情報を送信データに変換
                SetMyPiece(WHITE_PIECE);//自分の石色 　1:黒    2:白
                EnqueueData(2);
                SakuraIOSendData();
            }else if(buf==99) //ゲームリセットし、私の手番を黒にする。
            {
                GameReset();
                PieceAryChangeSendData();//盤情報を送信データに変換
                SetMyPiece(BLACK_PIECE);//自分の石色 　1:黒    2:白
                EnqueueData(2);
                SakuraIOSendData();
            }
        }
    }
    
    if(mysw5==1) //SW5 7側へ
    {
        LcdLED=1; //LCD_LED 点灯
    }else //SW5 6側へ
    {
        if(MyTurn==true) //自分のターンだったらLCD点灯
        {
            LcdLED=1; //LCD_LED 点灯
        }else
        {
            LcdLED=0; //LCD_LED 点灯
        }
    }
}

void CSakuraIOReversi::PrintLCD()
{
    Lcd.cls();
    Lcd.printf("X:%d Y:%d\r\n", MyAttackX,MyAttackY);
    if(MyTurn==true)
    {
        Lcd.printf("My %5d\r\n", sakuraioPoint);
    }else
    {
        Lcd.printf("You%5d\r\n", sakuraioPoint);
    }

    if(debug==true)
    {
        pc.printf("X:%d Y:%d\r\n", MyAttackX,MyAttackY);
        if(MyTurn==true)
        {
            pc.printf("My %5d\r\n", sakuraioPoint);
        }else
        {
            pc.printf("You%5d\r\n", sakuraioPoint);
        }
        
        for(int y=0;y<8;y++)
        {
            for(int x=0;x<8;x++)
            {
                pc.printf("%d  ", PieceAry[y][x]);
            }
            pc.printf("\r\n");
        }
    }
}

void CSakuraIOReversi::Run()
{
    SwitchCheck();
    SakuraIOReceptionData();
    PrintLCD();
    wait(0.01f);
} 

//こちらの手番の石判定処理
//判定する石座標(x,y)と置く石の色put_piece_colorを指定。判定のみ使用する場合はjudgeはtrue 石をひっくり返す場合はfalseを指定
bool CSakuraIOReversi::AttackJudge(int x,int y,int put_piece_color,bool judge)
{
    int MaxNumBuf=0;
    int MinNumBuf=0;
    int TurnPieceNum=0;
    
    int PutPieceColor=0;//置いた石の色
    int RivalPieceColor=0;//置いた石の色(PutPieceColor)と逆の色を指定する。置いた石にとっての相手石色
    
    PutPieceColor=put_piece_color;
    if(PutPieceColor==MyPiece)
    {
        RivalPieceColor=YourPiece;
    }else RivalPieceColor=MyPiece;

    //もし既に石が置かれていたら
    if(PieceAry[y][x]==PutPieceColor || PieceAry[y][x]==RivalPieceColor)
    {
        return false;
    }else
    {
        //右側探索
        if(x<6)//xが0～5ならば
        {
            if(PieceAry[y][x+1]==RivalPieceColor)//隣が相手石だったら
            {
                for(int i=x+2;i<8;i++)//2個目から探索開始
                {
                    if(PieceAry[y][i]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=x+1;j<i;j++)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[y][j]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[y][i]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //左側探索
        if(x>1) //xが2～7ならば
        {
            if(PieceAry[y][x-1]==RivalPieceColor)//隣が相手石だったら
            {
                for(int i=x-2;i>=0;i--)//2個目から探索開始
                {
                    if(PieceAry[y][i]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=x-1;j>=i;j--)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[y][j]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[y][i]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //下側探索
        if(y<6)//yが0～5ならば
        {
            if(PieceAry[y+1][x]==RivalPieceColor)//下が相手石だったら
            {
                for(int i=y+2;i<8;i++)//2個目から探索開始
                {
                    if(PieceAry[i][x]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=y+1;j<i;j++)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[j][x]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[i][x]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //上側探索
        if(y>1) //yが2～7ならば
        {
            if(PieceAry[y-1][x]==RivalPieceColor)//隣が相手石だったら
            {
                for(int i=y-2;i>=0;i--)//2個目から探索開始
                {
                    if(PieceAry[i][x]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=y-1;j>=i;j--)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[j][x]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[i][x]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //右下探索
        if(x<6 && y<6)//xが0～5かつyが0～5ならば
        {
            MaxNumBuf=MaxReturn(x,y);
            if(PieceAry[y+1][x+1]==RivalPieceColor)//右下が相手石だったら
            {
                for(int i=2;i<8-MaxNumBuf;i++)//2個目から探索開始
                {
                    if(PieceAry[y+i][x+i]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=1;j<i;j++)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[y+j][x+j]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[y+i][x+i]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //左上探索
        if(x>1 && y>1)//xが2～7かつyが2～7ならば
        {
            MinNumBuf=MinReturn(x,y);
            if(PieceAry[y-1][x-1]==RivalPieceColor)//左上が相手石だったら
            {
                for(int i=2;i<=MinNumBuf;i++)//2個目から探索開始
                {
                    if(PieceAry[y-i][x-i]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=1;j<i;j++)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[y-j][x-j]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[y-i][x-i]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //右上探索
        if(x<6 && y>1)//xが0～5かつyが2～7ならば
        {
            MinNumBuf=MinReturn(7-x,y);
            if(PieceAry[y-1][x+1]==RivalPieceColor)//右上が相手石だったら
            {
                for(int i=2;i<=MinNumBuf;i++)//2個目から探索開始
                {
                    if(PieceAry[y-i][x+i]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=1;j<i;j++)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[y-j][x+j]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[y-i][x+i]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        //左下探索
        if(x>1 && y<6)//xが2～7かつyが0～5ならば
        {
            MinNumBuf=MinReturn(x,7-y);
            if(PieceAry[y+1][x-1]==RivalPieceColor)//右上が相手石だったら
            {
                for(int i=2;i<=MinNumBuf;i++)//2個目から探索開始
                {
                    if(PieceAry[y+i][x-i]==PutPieceColor)//自分の石があったら
                    {
                        for(int j=1;j<i;j++)//そこまで全部自分の石色にする
                        {
                            if(judge==false) PieceAry[y+j][x-j]=PutPieceColor;
                            TurnPieceNum++;
                        }
                        break;
                    }else if(PieceAry[y+i][x-i]==0)//空白があったら抜ける
                    {
                        break;
                    }
                }
            }
        }
        
        if(TurnPieceNum==0)
        {
            return false;
        }else
        {
            if(judge==false) PieceAry[y][x]=PutPieceColor;
            return true;
        }
    }
}

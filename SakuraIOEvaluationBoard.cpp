#include "CSakuraIOEvaluationBoard.h"

CSakuraIOEvaluationBoard::CSakuraIOEvaluationBoard():
sakuraio(PC_9, PA_8),
Internal_i2c(PB_9, PB_8),
Lcd(Internal_i2c),
pc(PB_6, PB_7, 9600),
gps(PC_6,PC_7,9600),
gps_en(PA_11),
uart_buffer_index(0),
bme280(Internal_i2c),
internal_mpu9250_spi(PA_10,PA_12,PB_0),
mpu9250(internal_mpu9250_spi,PB_1),
mysw1(PC_14),mysw2(PH_0),mysw3(PC_0),mysw4(PC_1),mysw5(PC_2),mysw6(PC_3),
myLED1(PC_13),myLED2(PC_15),myLED3(PH_1),myLED4(PC_4),LcdLED(PC_5),sakuraioPoint(0)
{
}

CSakuraIOEvaluationBoard::~CSakuraIOEvaluationBoard()
{
}

void CSakuraIOEvaluationBoard::Run()
{
}
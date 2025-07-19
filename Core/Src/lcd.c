#include "lcd.h"
#include "font.h"
#include "main.h"

//SPI��ʾ���ӿ�
//LCD_RST
#define LCD_RST_SET
#define LCD_RST_RESET
//LCD_RS//dc
#define LCD_RS_SET      HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port,LCD_WR_RS_Pin,GPIO_PIN_SET)//PC4
#define LCD_RS_RESET    HAL_GPIO_WritePin(LCD_WR_RS_GPIO_Port,LCD_WR_RS_Pin,GPIO_PIN_RESET)
//LCD_CsprintfS
#define LCD_CS_SET      HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)
#define LCD_CS_RESET    HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)
//SPI Driver
#define SPI spi4
#define SPI_Drv (&hspi4)
//LCD_Brightness timer
#define LCD_Brightness_timer &htim1
#define LCD_Brightness_channel TIM_CHANNEL_2

static int32_t lcd_init(void);
static int32_t lcd_gettick(void);
static int32_t lcd_writereg(uint8_t reg,uint8_t* pdata,uint32_t length);
static int32_t lcd_readreg(uint8_t reg,uint8_t* pdata);
static int32_t lcd_senddata(uint8_t* pdata,uint32_t length);
static int32_t lcd_recvdata(uint8_t* pdata,uint32_t length);

ST7735_IO_t st7735_pIO = {
	lcd_init,
	NULL,
	NULL,
	lcd_writereg,
	lcd_readreg,
	lcd_senddata,
	lcd_recvdata,
	lcd_gettick
};

ST7735_Object_t st7735_pObj;
uint32_t st7735_id;

void LCD_Test(void)
{

	//uint8_t text[20];

	ST7735Ctx.Orientation = ST7735_ORIENTATION_LANDSCAPE_ROT180;
	ST7735Ctx.Panel = HannStar_Panel;
	ST7735Ctx.Type = ST7735_0_9_inch_screen;

	ST7735_RegisterBusIO(&st7735_pObj,&st7735_pIO);
	ST7735_LCD_Driver.Init(&st7735_pObj,ST7735_FORMAT_RBG565,&ST7735Ctx);
	ST7735_LCD_Driver.ReadID(&st7735_pObj,&st7735_id);
	ST7735_LCD_Driver.FillRect(&st7735_pObj, 0, 0, ST7735Ctx.Width,ST7735Ctx.Height, BLACK);

	/*
	LCD_SetBrightness(0);
	extern unsigned char WeActStudiologo_160_80[];
	ST7735_LCD_Driver.DrawBitmap(&st7735_pObj,0,0,WeActStudiologo_160_80);

  	uint32_t tick = HAL_GetTick();
	while (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) != GPIO_PIN_SET)
	{
		HAL_Delay(10);

		if (HAL_GetTick() - tick <= 1000)
			LCD_SetBrightness((HAL_GetTick() - tick) * 100 / 1000);
		else if (HAL_GetTick() - tick <= 3000)
		{
			sprintf((char *)&text, "%03d", (HAL_GetTick() - tick - 1000) / 10);
			LCD_ShowString(ST7735Ctx.Width - 30, 1, ST7735Ctx.Width, 16, 16, text);
			ST7735_LCD_Driver.FillRect(&st7735_pObj, 0, ST7735Ctx.Height - 3, (HAL_GetTick() - tick - 1000) * ST7735Ctx.Width / 2000, 3, 0xFFFF);
		}
		else if (HAL_GetTick() - tick > 3000)
			break;
	}
	while (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin) == GPIO_PIN_SET)
	{
		HAL_Delay(10);
	}

	sprintf((char *)&text, "WeAct Studio");
	LCD_ShowString(4, 4, ST7735Ctx.Width, 16, 16, text);
	sprintf((char *)&text, "STM32H7xx 0x%X", HAL_GetDEVID());
	LCD_ShowString(4, 22, ST7735Ctx.Width, 16, 16, text);
	sprintf((char *)&text, "LCD ID:0x%X", st7735_id);
	LCD_ShowString(4, 40, ST7735Ctx.Width, 16, 16, text);
*/
}

void LCD_SetBrightness(uint32_t Brightness)
{
	__HAL_TIM_SetCompare(LCD_Brightness_timer, LCD_Brightness_channel, Brightness);
}

uint32_t LCD_GetBrightness(void)
{
		return __HAL_TIM_GetCompare(LCD_Brightness_timer, LCD_Brightness_channel);
}

uint16_t POINT_COLOR=0xFFFF;	//������ɫ
uint16_t BACK_COLOR=BLACK;  //����ɫ
//��ָ��λ����ʾһ���ַ�
//x,y:��ʼ����
//num:Ҫ��ʾ���ַ�:" "--->"~"
//mode:���ӷ�ʽ(1)���Ƿǵ��ӷ�ʽ(0)

void LCD_ShowChar(uint16_t x,uint16_t y,uint8_t num)
{
  uint8_t temp,t1,t;
	uint16_t y0=y;
	uint16_t x0=x;
	uint16_t colortemp=POINT_COLOR;
  uint32_t h,w;

	uint16_t write[12][6];
	uint16_t count;

  ST7735_GetXSize(&st7735_pObj,&w);
	ST7735_GetYSize(&st7735_pObj,&h);

	//���ô���
	num=num-' ';//�õ�ƫ�ƺ��ֵ
	count = 0;

	for(t=0;t<12;t++)
	{
		temp=asc2_1206[num][t];
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)
				POINT_COLOR=(colortemp&0xFF)<<8|colortemp>>8;
			else
				POINT_COLOR=(BACK_COLOR&0xFF)<<8|BACK_COLOR>>8;

			write[count][t/2]=POINT_COLOR;
			count ++;
			if(count >= 12) count =0;

			temp<<=1;
			y++;
			if(y>=h){POINT_COLOR=colortemp;return;}//��������
			if((y-y0)==12)
			{
				y=y0;
				x++;
				if(x>=w){POINT_COLOR=colortemp;return;}//��������
				break;
			}
		}
	}

	ST7735_FillRGBRect(&st7735_pObj,x0,y0,(uint8_t *)&write);
	POINT_COLOR=colortemp;
}

//��ʾ�ַ���
//x,y:�������
//width,height:�����С
//*p:�ַ�����ʼ��ַ
void LCD_ShowString(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t *p)
{
	uint8_t x0=x;
	width+=x;
	height+=y;
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {
        if(x>=width){x=x0;y+=12;}
        if(y>=height)break;//�˳�
        LCD_ShowChar(x,y,*p);
        x+=6;
        p++;
    }
}

static int32_t lcd_init(void)
{
	int32_t result = ST7735_OK;
	HAL_TIMEx_PWMN_Start(LCD_Brightness_timer,LCD_Brightness_channel);
	return result;
}

static int32_t lcd_gettick(void)
{
	return HAL_GetTick();
}

static int32_t lcd_writereg(uint8_t reg,uint8_t* pdata,uint32_t length)
{
	int32_t result;
	LCD_CS_RESET;
	LCD_RS_RESET;
	result = HAL_SPI_Transmit(SPI_Drv,&reg,1,100);
	LCD_RS_SET;
	if(length > 0)
		result += HAL_SPI_Transmit(SPI_Drv,pdata,length,500);
	LCD_CS_SET;
	if(result>0){result = -1;}
	else{result = 0;}
	return result;
}

static int32_t lcd_readreg(uint8_t reg,uint8_t* pdata)
{
	int32_t result;
	LCD_CS_RESET;
	LCD_RS_RESET;

	result = HAL_SPI_Transmit(SPI_Drv,&reg,1,100);
	LCD_RS_SET;
	result += HAL_SPI_Receive(SPI_Drv,pdata,1,500);
	LCD_CS_SET;
	if(result>0){result = -1;}
	else{result = 0;}
	return result;
}

static int32_t lcd_senddata(uint8_t* pdata,uint32_t length)
{
	int32_t result;
	LCD_CS_RESET;
	//LCD_RS_SET;
	result =HAL_SPI_Transmit(SPI_Drv,pdata,length,100);
	LCD_CS_SET;
	if(result>0){result = -1;}
	else{result = 0;}
	return result;
}

static int32_t lcd_recvdata(uint8_t* pdata,uint32_t length)
{
	int32_t result;
	LCD_CS_RESET;
	//LCD_RS_SET;
	result = HAL_SPI_Receive(SPI_Drv,pdata,length,500);
	LCD_CS_SET;
	if(result>0){result = -1;}
	else{result = 0;}
	return result;
}


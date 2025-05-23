/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * Description of project
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ts.h"
#include "ts_calibration.h"


/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
 volatile int ChooseTimer = 0;
/* Private function prototypes -----------------------------------------------*/
static int GetUserButtonPressed(void);
static int GetTouchState (int *xCoord, int *yCoord);

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

/**
 * @brief  The application entry point.
 * @retval int
 */
void EXTI0_IRQHandler(void)
{
	 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	 ChooseTimer =ChooseTimer*(-1);
	 HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_13);

}
	int main(void)
	{
		/* MCU Configuration--------------------------------------------------------*/
		/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
		HAL_Init();
		/* Configure the system clock */
		SystemClock_Config();

		/* Initialize LCD and touch screen */
		LCD_Init();
		TS_Init(LCD_GetXSize(), LCD_GetYSize());
		/* touch screen calibration */
		//	TS_Calibration();

		/* Clear the LCD and display basic starter text */
		LCD_Clear(LCD_COLOR_BLACK);
		LCD_SetTextColor(LCD_COLOR_YELLOW);
		LCD_SetBackColor(LCD_COLOR_BLACK);
		LCD_SetFont(&Font20);
		// There are 2 ways to print text to screen: using printf or LCD_* functions
		LCD_DisplayStringAtLine(0, "  Ibrahimi 4AHET");
		// printf Alternative
		LCD_SetPrintPosition(1, 0);
		printf(" ---------------");
		LCD_SetPrintPosition(2, 0);
		printf(" EXTI Interrupt");

		LCD_SetFont(&Font8);
		LCD_SetColors(LCD_COLOR_MAGENTA, LCD_COLOR_BLACK); // TextColor, BackColor
		LCD_DisplayStringAtLineMode(39, "copyright xyz", CENTER_MODE);

		__HAL_RCC_GPIOA_CLK_ENABLE();
		GPIO_InitTypeDef user;
		user.Alternate = 0;
		user.Mode = GPIO_MODE_IT_RISING;
		user.Pin = GPIO_PIN_0;
		user.Pull = GPIO_PULLDOWN;
		user.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIOA, &user);


		__HAL_RCC_GPIOG_CLK_ENABLE();
		GPIO_InitTypeDef pg13;
		pg13.Alternate=0;
		pg13.Mode=GPIO_MODE_OUTPUT_PP;
		pg13.Pin=GPIO_PIN_13;
		pg13.Pull=GPIO_NOPULL;
		pg13.Speed=GPIO_SPEED_MEDIUM;
		HAL_GPIO_Init(GPIOG, &pg13);
		HAL_NVIC_EnableIRQ(EXTI0_IRQn);

		int cnt = 0;
		int cnt2 = 0;
		/* Infinite loop */
		while (1)
		{
			//execute main loop every 100ms
			HAL_Delay(100);
			cnt++;
			cnt2++;
			// display timer
			LCD_SetFont(&Font20);
			LCD_SetTextColor(LCD_COLOR_BLUE);
			LCD_SetPrintPosition(5, 0);
			printf("   Timer: %.1f", cnt/10.0);
			LCD_SetPrintPosition(7, 0);
			printf("Other Timer: %.1f", cnt2/10.0);

			if(ChooseTimer < 0){
				cnt++;
			}else if (ChooseTimer > 0) {
				cnt2++;
			}
			// test touch interface
			//		int x, y;
			//		if (GetTouchState(&x, &y)) {
			//			LCD_FillCircle(x, y, 5);
			//		}


		}
	}

	/**
	 * Check if User Button has been pressed
	 * @param none
	 * @return 1 if user button input (PA0) is high
	 */
	static int GetUserButtonPressed(void) {
		return (GPIOA->IDR & 0x0001);
	}

	/**
	 * Check if touch interface has been used
	 * @param xCoord x coordinate of touch event in pixels
	 * @param yCoord y coordinate of touch event in pixels
	 * @return 1 if touch event has been detected
	 */
	static int GetTouchState (int* xCoord, int* yCoord) {
		void    BSP_TS_GetState(TS_StateTypeDef *TsState);
		TS_StateTypeDef TsState;
		int touchclick = 0;

		TS_GetState(&TsState);
		if (TsState.TouchDetected) {
			*xCoord = TsState.X;
			*yCoord = TsState.Y;
			touchclick = 1;
			if (TS_IsCalibrationDone()) {
				*xCoord = TS_Calibration_GetX(*xCoord);
				*yCoord = TS_Calibration_GetY(*yCoord);
			}
		}

		return touchclick;
	}



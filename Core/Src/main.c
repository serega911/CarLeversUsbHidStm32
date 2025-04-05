/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include "usbd_customhid.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define USB_DELAY 200

#define STATE_OFF   0
#define STATE_ON    1
#define STATE_IND   1
#define STATE_INT   1
#define STATE_LEFT  1
#define STATE_LOW   2
#define STATE_LO    2
#define STATE_RIGHT 2
#define STATE_HI    3

// Blinkers, Fog
typedef struct
{
	GPIO_TypeDef *gpio;
	uint16_t pin;
	uint8_t current_state;
	uint16_t button;
} HandlerGeneric_t;

typedef struct
{
	GPIO_TypeDef *gpio;
	uint16_t pin_left;
	uint16_t pin_right;
	uint8_t current_state;
	uint16_t button_left;
	uint16_t button_right;
	uint32_t enable_timestamp;
	bool lazy;
} HandlerBlinkers_t;

// Indication light, Low beam
typedef struct
{
	GPIO_TypeDef *gpio;
	uint16_t pin_ind;
	uint16_t pin_low;
	uint8_t current_state;
	uint16_t button;
} HandlerLights_t;

// Front wipers
typedef struct
{
	GPIO_TypeDef *gpio;
	uint16_t pin_int;
	uint16_t pin_lo;
	uint16_t pin_hi;
	uint8_t current_state;
	uint16_t button_up;
	uint16_t button_down;
} HandlerWipers_t;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// Button is pressed while lever is in corresponding position
static HandlerGeneric_t handlers_type0[] = {
		{.gpio = GPIOA, .pin = GPIO_PIN_4, .current_state = STATE_OFF, .button = 1 << 4},  // High beam
		{.gpio = GPIOA, .pin = GPIO_PIN_5, .current_state = STATE_OFF, .button = 1 << 5},  // Front Fog lamps
		{.gpio = GPIOA, .pin = GPIO_PIN_6, .current_state = STATE_OFF, .button = 1 << 6},  // Rear Fog lamps
		{.gpio = GPIOB, .pin = GPIO_PIN_7, .current_state = STATE_OFF, .button = 1 << 11}, // Spray FRONT
		{.gpio = GPIOB, .pin = GPIO_PIN_8, .current_state = STATE_OFF, .button = 1 << 12}  // Spray REAR
};

// Button is pressed when lever enters and exits corresponding position
static HandlerGeneric_t handlers_type1[] = {
		{.gpio = GPIOB, .pin = GPIO_PIN_6, .current_state = STATE_OFF, .button = 1 << 10}, // Wipers REAR
};

// Custom behavior to handle blinkers logic
static HandlerBlinkers_t handler_blinkers = {
		.gpio = GPIOA, .pin_left = GPIO_PIN_0, .pin_right = GPIO_PIN_1, .current_state = STATE_OFF,
		.button_left = 1 << 0, .button_right = 1 << 1, .enable_timestamp = 0, .lazy = false
};

// Custom behavior to handle lights logic
static HandlerLights_t handler_low_beam = {
		.gpio = GPIOA, .pin_ind = GPIO_PIN_2, .pin_low = GPIO_PIN_3, .current_state = STATE_OFF, .button = 1 << 2
};

// Custom behavior to handle wipers logic
static HandlerWipers_t handler_wipers = { .gpio = GPIOB, .pin_int = GPIO_PIN_3,
		.pin_lo = GPIO_PIN_4, .pin_hi = GPIO_PIN_5, .current_state = STATE_OFF,
		.button_up = 1 << 7, .button_down = 1 << 8
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern USBD_HandleTypeDef hUsbDeviceFS;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  static const int handlers_type0_size = sizeof(handlers_type0)/sizeof(handlers_type0[0]);
  static const int handlers_type1_size = sizeof(handlers_type1)/sizeof(handlers_type1[0]);

  while (1)
  {
	  /*uint16_t report_val = 0;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) ? 0 : 1 << 0;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) ? 0 : 1 << 1;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2) ? 0 : 1 << 2;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_3) ? 0 : 1 << 3;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) ? 0 : 1 << 4;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_5) ? 0 : 1 << 5;
	  report_val |= HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_6) ? 0 : 1 << 6;
	  report_val |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) ? 0 : 1 << 7;
	  report_val |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) ? 0 : 1 << 8;
	  report_val |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) ? 0 : 1 << 9;
	  report_val |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_6) ? 0 : 1 << 10;
	  report_val |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7) ? 0 : 1 << 11;
	  report_val |= HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) ? 0 : 1 << 12;
	  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&report_val, 2);
	  HAL_Delay(USB_DELAY);*/

	  // Button is pressed while GPIO is active
	  uint16_t report_val = 0;
	  for(int i = 0; i < handlers_type0_size; ++i)
	  {
		  HandlerGeneric_t* handler = handlers_type0 + i;
		  report_val |= HAL_GPIO_ReadPin(handler->gpio, handler->pin) ? 0 : handler->button;
	  }

	  // Button is pressed when GPIO state changes
	  for(int i = 0; i < handlers_type1_size; ++i)
	  {
		  HandlerGeneric_t* handler = handlers_type1 + i;
		  uint8_t new_state = HAL_GPIO_ReadPin(handler->gpio, handler->pin) ? STATE_OFF : STATE_ON;
		  if(new_state != handler->current_state)
		  {
			  handler->current_state = new_state;
			  report_val |= handler->button;
		  }
		  else
		  {
			  report_val &= ~handler->button;
		  }
	  }

	  // Blinkers
	  {
		  uint8_t new_state = HAL_GPIO_ReadPin(handler_blinkers.gpio, handler_blinkers.pin_left) ? STATE_OFF : STATE_LEFT;
		  new_state = HAL_GPIO_ReadPin(handler_blinkers.gpio, handler_blinkers.pin_right) ? new_state : STATE_RIGHT;
		  const uint32_t ms = HAL_GetTick();

		  if(STATE_OFF == new_state && 0 != handler_blinkers.enable_timestamp)
		  {
				const uint32_t ms_passed = ms - handler_blinkers.enable_timestamp;
				if(ms_passed < 1000)
				{
					handler_blinkers.lazy = true;
					new_state = handler_blinkers.current_state;
				}
				else if(handler_blinkers.lazy && ms_passed < 3100)
				{
					new_state = handler_blinkers.current_state;
				}
				else
				{
					handler_blinkers.lazy = false;
				}
		  }

		  if(new_state != handler_blinkers.current_state)
		  {
			  if(STATE_OFF == new_state)
			  {
				  report_val |= (STATE_LEFT == handler_blinkers.current_state ? handler_blinkers.button_left : handler_blinkers.button_right);
				  handler_blinkers.enable_timestamp = 0;
			  }
			  else if(STATE_LEFT == new_state)
			  {
				  report_val |= handler_blinkers.button_left;
				  handler_blinkers.enable_timestamp = ms;
			  }
			  else if(STATE_RIGHT == new_state)
			  {
				  report_val |= handler_blinkers.button_right;
				  handler_blinkers.enable_timestamp = ms;
			  }
			  handler_blinkers.current_state = new_state;
		  }
		  else
		  {
			  report_val &= ~handler_blinkers.button_left;
			  report_val &= ~handler_blinkers.button_right;
		  }
	  }

	  // Low beam, indication light
	  {
		  uint8_t new_state = HAL_GPIO_ReadPin(handler_low_beam.gpio, handler_low_beam.pin_ind) ? STATE_OFF : STATE_IND;
		  new_state = HAL_GPIO_ReadPin(handler_low_beam.gpio, handler_low_beam.pin_low) ? new_state : STATE_LOW;
		  if(new_state != handler_low_beam.current_state)
		  {
			  handler_low_beam.current_state = (handler_low_beam.current_state == STATE_LOW ? STATE_OFF : handler_low_beam.current_state + 1);
			  report_val |= handler_low_beam.button;
		  }
	  }

	  // Front wipers
	  {
		  uint8_t new_state = handler_wipers.current_state;
		  if(!HAL_GPIO_ReadPin(handler_wipers.gpio, handler_wipers.pin_int))
		  {
			  new_state = STATE_INT;
		  }
		  else if(!HAL_GPIO_ReadPin(handler_wipers.gpio, handler_wipers.pin_lo))
		  {
			  new_state = STATE_LO;
		  }
		  else if(!HAL_GPIO_ReadPin(handler_wipers.gpio, handler_wipers.pin_hi))
		  {
			  new_state = STATE_HI;
		  }
		  else
		  {
			  new_state = STATE_OFF;
		  }

		  if(new_state > handler_wipers.current_state)
		  {
			  report_val |= handler_wipers.button_up;
			  ++handler_wipers.current_state;
		  }
		  else if(new_state < handler_wipers.current_state)
		  {
			  report_val |= handler_wipers.button_down;
			  --handler_wipers.current_state;
		  }
	  }

	  // Send buttons
	  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&report_val, 2);
	  HAL_Delay(USB_DELAY);

	  // Unset Low Beam button
	  if(report_val & handler_low_beam.button)
	  {
		  report_val &= ~handler_low_beam.button;
		  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&report_val, 2);
		  HAL_Delay(USB_DELAY);
	  }

	  // Unset Wipers UP button
	  if(report_val & handler_wipers.button_up)
	  {
		  report_val &= ~handler_wipers.button_up;
		  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&report_val, 2);
		  HAL_Delay(USB_DELAY);
	  }

	  // Unset Wipers DOWN button
	  if(report_val & handler_wipers.button_down)
	  {
		  report_val &= ~handler_wipers.button_down;
		  USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, (uint8_t*)&report_val, 2);
		  HAL_Delay(USB_DELAY);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : BLINK_1_Pin BLINK_2_Pin MARKERS_Pin LOW_BEAM_Pin
                           HIGH_BEAM_Pin FOG_FRONT_Pin FOG_REAR_Pin */
  GPIO_InitStruct.Pin = BLINK_1_Pin|BLINK_2_Pin|MARKERS_Pin|LOW_BEAM_Pin
                          |HIGH_BEAM_Pin|FOG_FRONT_Pin|FOG_REAR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : WIPERS_INT_Pin WIPERS_LO_Pin WIPERS_HI_Pin WIPERS_REAR_Pin
                           SPRAY_FRONT_Pin SPRAY_REAR_Pin */
  GPIO_InitStruct.Pin = WIPERS_INT_Pin|WIPERS_LO_Pin|WIPERS_HI_Pin|WIPERS_REAR_Pin
                          |SPRAY_FRONT_Pin|SPRAY_REAR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

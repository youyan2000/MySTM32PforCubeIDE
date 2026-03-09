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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define P0 	0	// 休止符频率

#define L1 262  // 低音频率
#define RL1 277
#define L2 294
#define RL2 311
#define L3 330
#define L4 349
#define RL4 370
#define L5 392
#define RL5 415
#define L6 440
#define RL6 466
#define L7 494

#define M1 523  // 中音频率
#define M2 587
#define M3 659
#define M4 698
#define M5 784
#define M6 880
#define M7 988

#define H1 1047 // 高音频率
#define H2 1175
#define H3 1319
#define H4 1397
#define H5 1568
#define H6 1760
#define H7 1976

typedef struct
{
    uint16_t frequency;
    float period;
} Bate;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
const Bate MoChouXiang[] = {
    {L5, 0.5f}, {L5, 0.5f}, {RL5,  0.5f},
	{L6, 1}, {M1, 0.5f}, {L6, 0.5f}, {M1, 0.5f}, {L6, 0.5f}, {L6, 0.5f}, {RL5, 0.5f},
	{L5, 0.5f}, {M2, 0.5f}, {L5, 0.5f}, {L5, 0.5f}, {L5, 0.5f}, {M2, 0.5f}, {L5, 0.5f}, {RL5, 0.5f},
	{L6, 1}, {M1, 0.5f}, {L6, 0.5f}, {M1, 0.5f}, {L6, 0.5f}, {L6, 0.5f}, {RL5, 0.5f},
	{L5, 0.5f}, {M2, 0.5f}, {L5, 0.5f}, {L5, 0.5f}, {L5, 0.5f}, {M2, 0.5f}, {L5, 0.5f}, {RL5, 0.5f},
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
 * 计算定时器计数频率
 */
uint32_t TIM_GetCounterFreq(TIM_HandleTypeDef *htim) {
    uint32_t timer_clock;
    // 高级定时器是APB2
    if (htim->Instance == TIM1) {
        timer_clock = HAL_RCC_GetPCLK2Freq();
        // 如果APB分频不为1，定时器时钟会翻倍
        if (HAL_RCC_GetPCLK2Freq() != (HAL_RCC_GetHCLKFreq() / 1)) {
            timer_clock *= 2;
        }
    } else {
    	// 其他定时器是APB1
        timer_clock = HAL_RCC_GetPCLK1Freq();
        // 如果APB分频不为1，定时器时钟会翻倍
        if (HAL_RCC_GetPCLK1Freq() != (HAL_RCC_GetHCLKFreq() / 1)) {
            timer_clock *= 2;
        }
    }

    uint32_t prescaler = htim->Instance->PSC;
    return timer_clock / (prescaler + 1);
}
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
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  // 开始PWM输出
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  // TIM4的计数频率
  uint32_t timFrequency = TIM_GetCounterFreq(&htim4);
  // 播放状态
  uint8_t playState = 0;
  // 播放进度
  uint32_t playIndex = 0;
  // 节拍速度(每分钟多少拍)
  uint8_t bpm = 132;
  // 每拍的持续时间
  float noteDuration = 1000 * 60 / bpm;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // 按键检测，切换播放与暂停
	  if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET){
		  HAL_Delay(10);
		  if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET){
			  playState = !playState;
			  while(HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET);
		  }
	  }
	  // 播放
	  if (playState){
		  const Bate bate = MoChouXiang[playIndex];
		  if (bate.frequency == P0) {
			  // 休止符
			  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
		  } else {
			  // 将频率转换为计数值, 设置到自动重装载寄存器
			  uint32_t arr = timFrequency / bate.frequency;
			  __HAL_TIM_SET_AUTORELOAD(&htim4,arr);
			  // 设置占空比为20%
			  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, arr / 5); // 20%占空比
			  // 从0开始计数 重置PWM波形
			  __HAL_TIM_SetCounter(&htim4, 0);
		  }
		  // 延时该音符的持续时间 (5ms的空白以区分连续两个相同的音符)
		  HAL_Delay((uint32_t) (bate.period * noteDuration) - 5);
		  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
		  HAL_Delay(5);

		  // 下一个音符
		  playIndex++;
		  // 播放结束
		  if (playIndex >= sizeof(MoChouXiang)){
			  playState = 0;
			  playIndex = 0;
		  }
	  }else{
		  __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, 0);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 72 - 1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin : KEY1_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStruct);

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
#ifdef USE_FULL_ASSERT
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

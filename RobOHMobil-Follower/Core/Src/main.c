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
#include "VL53L4CD_api.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */
int IR_Sensor_R;						// Rechter IR sieht IR-Beacon
int IR_Sensor_L;						// Linker IR sieht IR-Beacon

// TOF Sensor Devices used in Driver Functions
Dev_t TOF_R   = 0x54;   	// Neue Adresse für R
Dev_t TOF_L   = 0x58;  		// Neue Adresse für L

VL53L4CD_ResultsData_t result_R;
VL53L4CD_ResultsData_t result_L;



int16_t Range_R;
int16_t Range_Last_R;
int16_t Range_L;
int16_t Range_Last_L;
int16_t Range_Reference = 100;
int16_t R_Speed = 0;
int16_t L_Speed = 0;
static int integral = 0;


int16_t CalibrateOffset_R;
int16_t CalibrateOffset_L;
int16_t CalibratedOffset_R;
int16_t CalibratedOffset_L;

int Error_Count_L = 0;
int Error_Count_R = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
//INIT
void Custom_GPIO_Init();
static void VL53L4CD_Init(void);

//READ
static void Read_TOF(void);
static int Read_IR_Sensor(void);

//WRITE
static void Drive_Follow(void);
static void Drive_Turn_Right(void);
static void Drive_Turn_Left(void);
static void Drive_Stop(void);
static void Set_Speed_L(int speed);
static void Set_Speed_R(int speed);
static void Set_RGB(int R, int G, int B);

//CALCULATE
int calc_speed_to_pwm(int s);
static int Check_Valid_Range(void);

//TEST
void VL53L4CD_I2C_Test(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */



  VL53L4CD_Init();
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

  Set_RGB(0, 0, 1);		// Blaue LED => Sucht nach IR signal
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  Read_TOF();
	  Drive_Follow();

	  if(Check_Valid_Range()) {
		  Set_RGB(0, 1, 0);
	  } else {
		  Set_RGB(1, 0, 0);
	  }

	  /*
	  Read_TOF();
	  L_Speed = 0;
	  R_Speed = 0;
	  if (Read_IR_Sensor()) {		// Wenn Beide erkennen, fahren

		  while(Check_Valid_Range()) {
			  Set_RGB(0, 1, 0); 		// Grün => Folgemodus
 			  Read_TOF();
 		  	  Drive_Follow();
		  }

	  } else if (IR_Sensor_L) {		// Links drehen
		  Drive_Turn_Left();
		  Set_RGB(1, 1, 0); 		// Rot + Grün = Gelb => nur ein IR erkennt etwas
	  } else if (IR_Sensor_R) {		// Rechts drehen
		  Drive_Turn_Right();
		  Set_RGB(1, 1, 0); 		// Rot + Grün = Gelb => nur ein IR erkennt etwas
	  } else {						// Rechts Drehen + Blau  -> Suchen
		  Drive_Turn_Right();
		  Set_RGB(0, 0, 1);		// Blaue LED => Sucht nach IR signal
	  }

*/




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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x4052060F;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }

  /** I2C Fast mode Plus enable
  */
  HAL_I2CEx_EnableFastModePlus(I2C_FASTMODEPLUS_I2C1);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */
  	  	  	  	  	  	  	  	  	  	  	  	  //ARR 10000 -> ca. 17kHz Freq + guter wert für CCR =>> CCR = 0 -> no speed; CCR = 10000 -> max speed
  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 10000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */
  	  	  	  	  	  	  	  	  	  	  	  	  //ARR 10000 -> ca. 17kHz Freq + guter wert für CCR =>> CCR = 0 -> no speed; CCR = 10000 -> max speed
  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 10000;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 4473;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 2237;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

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
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, L_Forward_Pin|L_Backward_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, R_Forward_Pin|R_Backward_Pin|TOF_R_EN_Pin|TOF_L_EN_Pin
                          |RGB_Red_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, RGB_Blue_Pin|RGB_Green_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : L_Forward_Pin L_Backward_Pin */
  GPIO_InitStruct.Pin = L_Forward_Pin|L_Backward_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : R_Forward_Pin R_Backward_Pin TOF_R_EN_Pin TOF_L_EN_Pin
                           RGB_Red_Pin */
  GPIO_InitStruct.Pin = R_Forward_Pin|R_Backward_Pin|TOF_R_EN_Pin|TOF_L_EN_Pin
                          |RGB_Red_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TOF_L_EXTI_3_Pin TOF_R_EXTI7_Pin */
  GPIO_InitStruct.Pin = TOF_L_EXTI_3_Pin|TOF_R_EXTI7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : IR_Sensor_R_Pin IR_Sensor_L_Pin */
  GPIO_InitStruct.Pin = IR_Sensor_R_Pin|IR_Sensor_L_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : RGB_Blue_Pin RGB_Green_Pin LD2_Pin */
  GPIO_InitStruct.Pin = RGB_Blue_Pin|RGB_Green_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

//------------------------------------------------------------------------------------
//		USER INIT
//------------------------------------------------------------------------------------

void Custom_GPIO_Init()
{
	GPIOA->MODER &= ~(0x03 << 4); 	//Reset GPIOA_MODE2
	GPIOA->MODER |= (0x01 << 4); 	//Set GPIOA_MODE2
	GPIOA->MODER &= ~(0x03 << 8); 	//Reset GPIOA_MODE4
	GPIOA->MODER |= (0x01 << 8);	//Set GPIOA_MODE4 to output mode
}

static void VL53L4CD_Init(void) {

	HAL_GPIO_WritePin(GPIOA, TOF_R_EN_Pin | TOF_L_EN_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);

	Dev_t default_addr = 0x52;   	// Default

	// INIT TOF_R
	HAL_GPIO_WritePin(GPIOA, TOF_R_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(10);

	if (VL53L4CD_SetI2CAddress(default_addr, TOF_R) != 0) {
		Error_Handler();
	}
	HAL_Delay(10);

	if (VL53L4CD_SensorInit(TOF_R) != 0) {
        Error_Handler();
    }

	// INIT TOF_L
	//HAL_GPIO_WritePin(GPIOA, TOF_R_EN_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(GPIOA, TOF_L_EN_Pin, GPIO_PIN_SET);

	HAL_Delay(10);


	if (VL53L4CD_SetI2CAddress(default_addr, TOF_L) != 0) {
        Error_Handler();
    }
	HAL_Delay(10);

	if (VL53L4CD_SensorInit(TOF_L) != 0) {
        Error_Handler();
    }

	HAL_GPIO_WritePin(GPIOA, TOF_R_EN_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	//Nur bei Anfangs Kalibrierung
/*
	VL53L4CD_CalibrateOffset(TOF_R, 100, &CalibrateOffset_R, 20);
	VL53L4CD_CalibrateOffset(TOF_L, 100, &CalibrateOffset_L, 20);
	VL53L4CD_GetOffset(TOF_R, &CalibratedOffset_R);
	VL53L4CD_GetOffset(TOF_L, &CalibratedOffset_L);
*/

	VL53L4CD_SetOffset(TOF_R, -10);		// Wert eingeben je nach Sensor, ist beschriftet
	VL53L4CD_SetOffset(TOF_L, -20);
	VL53L4CD_StartRanging(TOF_R);
	VL53L4CD_StartRanging(TOF_L);
}

//------------------------------------------------------------------------------------
//		USER TEST
//------------------------------------------------------------------------------------

void VL53L4CD_I2C_Test(void)
{
    uint8_t id = 0;
    HAL_StatusTypeDef ret;

    // XSHUT sicher aktivieren
    HAL_GPIO_WritePin(GPIOA, TOF_R_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, TOF_L_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    if (VL53L4CD_SetI2CAddress(0x52, TOF_L) != 0) {
            Error_Handler();
        }
    // Lese Model-ID-Register (0x010F)
    ret = HAL_I2C_Mem_Read(&hi2c1,
    					   TOF_L,          // Default-Adresse (0x29 << 1)
                           0x010F,
                           I2C_MEMADD_SIZE_16BIT,
                           &id, 1, 100);

    if (ret == HAL_OK)
    {
        // Sensor antwortet -> I2C OK
        HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_SET); // z.B. LED an
    }
    else
    {
        // Kein ACK -> Hardware / Adresse / XSHUT / Pull-Ups
        HAL_GPIO_WritePin(GPIOB, LD2_Pin, GPIO_PIN_RESET); // LED aus
        Error_Handler(); // oder Breakpoint setzen
    }
}

//------------------------------------------------------------------------------------
//		USER READ
//------------------------------------------------------------------------------------

/* Liest die IR_Sensoren auf Variablen ein,  returned 1 wenn beide etwas erkennen	*/
static int Read_IR_Sensor(void)
{
	IR_Sensor_R = !HAL_GPIO_ReadPin(GPIOA, IR_Sensor_R_Pin);
	IR_Sensor_L = !HAL_GPIO_ReadPin(GPIOA, IR_Sensor_L_Pin);

	if(IR_Sensor_R && IR_Sensor_L) {									//TEST LED für IR-Test
		HAL_GPIO_WritePin(GPIOB, LD2_Pin, 1);
	} else {
		HAL_GPIO_WritePin(GPIOB, LD2_Pin, 0);
	}
	
	return (IR_Sensor_R && IR_Sensor_L);
}

static void Read_TOF(void) 
{
	uint8_t dataReady_R = 0;
	uint8_t dataReady_L = 0;
	
	if (VL53L4CD_CheckForDataReady(TOF_R, &dataReady_R) != 0) {
		Error_Handler();
	}

	if (VL53L4CD_CheckForDataReady(TOF_L, &dataReady_L) != 0) {
		Error_Handler();
	}

	if (dataReady_R && dataReady_L)
	{
		Range_Last_R = Range_R;
		Range_Last_L = Range_L;
		
		if (VL53L4CD_GetResult(TOF_R, &result_R) != 0) {
			Error_Handler();
		}

		if (VL53L4CD_ClearInterrupt(TOF_R) != 0) {
			Error_Handler();
		}

		if(result_R.range_status == 0) {
			Range_R = result_R.distance_mm;
			Error_Count_R = 0;
		} else {
			Error_Count_R++;
		}





 	 	if (VL53L4CD_GetResult(TOF_L, &result_L) != 0) {
			Error_Handler();
		}

 	 	if (VL53L4CD_ClearInterrupt(TOF_L) != 0) {
			Error_Handler();
		}

 	 	if(result_L.range_status == 0) {
 	 		Range_L = result_L.distance_mm;
 	 		Error_Count_L = 0;
 	 	} else {
 	 		Error_Count_L++;
 	 	}



 	 	if((Error_Count_L > 10)||(Error_Count_R > 10)) {
 	 		Set_RGB(1, 1, 1);
 	 		Drive_Stop();
 	 		Error_Handler();
 	 	}



		if (VL53L4CD_StartRanging(TOF_R) != 0) {
			Error_Handler();
		}

		if (VL53L4CD_StartRanging(TOF_L) != 0) {
			Error_Handler();
		}

	}

	HAL_Delay(5);
}

//------------------------------------------------------------------------------------
//		USER WRITE
//------------------------------------------------------------------------------------

static void Drive_Follow(void)
{
	float Ki = 1.5;
	float Kt = 2.0;


	int distance = (Range_L + Range_R) / 2;
	int error = Range_Reference - distance;

	integral += error;

	if(integral > 2000) integral = 2000;
	if(integral < -2000) integral = -2000;

	int speed = Ki * integral;

	int turn = Kt * (Range_L - Range_R);

	int L_Speed = speed - turn;
	int R_Speed = speed + turn;

	L_Speed = CLAMP(L_Speed, -4000, 4000);
	R_Speed = CLAMP(R_Speed, -4000, 4000);

	Set_Speed_L(calc_speed_to_pwm(L_Speed));
	Set_Speed_R(calc_speed_to_pwm(R_Speed));


	/*//Calculate Speed
	if((Range_R + 15) < Range_Reference) {		//Sind wir unter unserer Referenz abzüglich von Toleranzen

		if(Range_R <= Range_Last_R) {
			R_Speed -= 2;
					//(Range_Reference - Range_R);
		}

	} else if((Range_R - 15) > Range_Reference) {	//Sind wir über unserer Referenz abzüglich von Toleranzen

		if(Range_R >= Range_Last_R) {
			R_Speed += 2;
			//(Range_R - Range_Reference);
		}
	} else {

		if(Range_R < Range_Last_R) {
			R_Speed -= 1;
		} else if(Range_R > Range_Last_R) {
			R_Speed += 1;
		}
	}



	if(R_Speed > 4000) {
		R_Speed = 4000;
	} else if(R_Speed < -4000) {
		R_Speed = -4000;
	}

	if((Range_L + 15) < Range_Reference) {		//Sind wir unter unserer Referenz abzüglich von Toleranzen

		if(Range_L <= Range_Last_L) {
			L_Speed -= 2;
			//(Range_Reference - Range_L);
		}

	} else if((Range_L - 15) > Range_Reference) {	//Sind wir über unserer Referenz abzüglich von Toleranzen

		if(Range_L >= Range_Last_L) {
			L_Speed += 2;
			//(Range_L - Range_Reference);
		}

	} else {

		if(Range_L < Range_Last_L) {
			L_Speed -= 1;
		} else if(Range_L > Range_Last_L) {
			L_Speed += 1;
		}
	}

	if(L_Speed > 4000) {
		L_Speed = 4000;
	} else if(L_Speed < -4000) {
		L_Speed = -4000;
	}

	if(R_Speed > 50) {
		Set_Speed_R(R_Speed + 6000);				//Speed muss Effektiv mind PWM von 60% haben um genug Drehmoment zu haben damit das  Fahrzeug sich bewegt
	} else if(R_Speed < -50) {
		Set_Speed_R(R_Speed - 6000);
	} else {
		Set_Speed_R(0);
	}


	if(L_Speed > 50) {
		Set_Speed_L(L_Speed + 6000);
	} else if(L_Speed < -50) {
		Set_Speed_L(L_Speed - 6000);
	} else {
		Set_Speed_L(0);
	}

	HAL_Delay(25);*/


}

static void Drive_Turn_Right(void)
{
	Set_Speed_L(6000);		//60% Speed
	Set_Speed_R(-6000);
}

static void Drive_Turn_Left(void)
{
	Set_Speed_L(-6000);		//60% Speed
	Set_Speed_R(6000);
}

static void Drive_Stop(void)
{
	Set_Speed_L(0);
	Set_Speed_R(0);
}


// Speed 0 - 10000
static void Set_Speed_L(int speed)
{
	if(speed == 0) {

		HAL_GPIO_WritePin(L_Backward_GPIO_Port, L_Backward_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(L_Forward_GPIO_Port , L_Forward_Pin , GPIO_PIN_RESET);

		TIM1->CCR1 = speed;

	} else if(speed <= 10000 && speed > 0) {

		HAL_GPIO_WritePin(L_Backward_GPIO_Port, L_Backward_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(L_Forward_GPIO_Port , L_Forward_Pin , GPIO_PIN_SET  );

		TIM1->CCR1 = speed;

	} else if(speed >= -10000 && speed < 0) {

		HAL_GPIO_WritePin(L_Forward_GPIO_Port , L_Forward_Pin , GPIO_PIN_RESET);
		HAL_GPIO_WritePin(L_Backward_GPIO_Port, L_Backward_Pin, GPIO_PIN_SET  );

		TIM1->CCR1 = -speed;

	} else {
		Error_Handler();
	}
}

// Speed 0 - 10000
static void Set_Speed_R(int speed)
{
	if(speed == 0) {

			HAL_GPIO_WritePin(R_Backward_GPIO_Port, R_Backward_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R_Forward_GPIO_Port , R_Forward_Pin , GPIO_PIN_RESET);

			TIM2->CCR2 = speed;

		} else if(speed <= 10000 && speed > 0) {

			HAL_GPIO_WritePin(R_Backward_GPIO_Port, R_Backward_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R_Forward_GPIO_Port , R_Forward_Pin , GPIO_PIN_SET  );

			TIM2->CCR2 = speed;

		} else if(speed >= -10000 && speed < 0) {

			HAL_GPIO_WritePin(R_Forward_GPIO_Port , R_Forward_Pin , GPIO_PIN_RESET);
			HAL_GPIO_WritePin(R_Backward_GPIO_Port, R_Backward_Pin, GPIO_PIN_SET  );

			TIM2->CCR2 = -speed;

		} else {
			Error_Handler();
		}
}

static void Set_RGB(int R, int G, int B) {

	// Eingabe 1 um jeweilige LED an zumachen
	// LEDs sind Low Aktiv -> 1 = Reset = Pin LOW

	if(R >= 1) {
		HAL_GPIO_WritePin(RGB_Red_GPIO_Port, RGB_Red_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(RGB_Red_GPIO_Port, RGB_Red_Pin, GPIO_PIN_SET);
	}

	if(G >= 1) {
		HAL_GPIO_WritePin(RGB_Green_GPIO_Port, RGB_Green_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(RGB_Green_GPIO_Port, RGB_Green_Pin, GPIO_PIN_SET);
	}

	if(B >= 1) {
		HAL_GPIO_WritePin(RGB_Blue_GPIO_Port, RGB_Blue_Pin, GPIO_PIN_RESET);
	} else {
		HAL_GPIO_WritePin(RGB_Blue_GPIO_Port, RGB_Blue_Pin, GPIO_PIN_SET);
	}
}



//------------------------------------------------------------------------------------
//		USER CALCULATE
//------------------------------------------------------------------------------------

int calc_speed_to_pwm(int s) {
	if(s > 50)
		return s + 6000;
	else if(s < -50)
		return s - 6000;
	else
		return 0;
}

// Returns 1 when TOF-Ranges are valid ( >0; <1000)
static int Check_Valid_Range(void)
{
	if(
		Range_R <= 5   ||
		Range_R > 500 ||
		Range_L <= 5   ||
		Range_L > 500
	   )
	{
		return 0;
	} else
	{
		return 1;
	}
}



/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
	Set_RGB(1, 0, 0); 		// Rot => Fehler
	__disable_irq();
	Drive_Stop();
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

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
    STATE_IDLE,
    STATE_MENU,
    STATE_SUBMENU,
    STATE_RUN,
    STATE_DIAGNOSTIC,
    STATE_ERROR
} FSMState;

typedef enum {
    EVENT_NINGUNO,
    EVENT_PUSH,
    EVENT_H,
    EVENT_AH,
    EVENT_ERROR
} FSMEvent;

typedef struct {
    FSMState estado;
    uint8_t pagina;
} screenMsg_t;

typedef enum {
	PAG_CONFIG,		//0
	PAG_DIAG,		//1
	PAG_RUN,		//2
	PAG_RES,		//3
	PAG_CAP,		//4
	PAG_T1,			//5
	PAG_T2,			//6
	PAG_T3,			//7
	PAG_T4,			//8
	PAG_HEAP,		//9
	PAG_FACU		//10
}page_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

/* Definitions for task_DISPLAY */
osThreadId_t task_DISPLAYHandle;
const osThreadAttr_t task_DISPLAY_attributes = {
  .name = "task_DISPLAY",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_GUI */
osThreadId_t Task_GUIHandle;
const osThreadAttr_t Task_GUI_attributes = {
  .name = "Task_GUI",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_DIAGNOSTIC */
osThreadId_t Task_DIAGNOSTICHandle;
const osThreadAttr_t Task_DIAGNOSTIC_attributes = {
  .name = "Task_DIAGNOSTIC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Task_PROCESSING */
osThreadId_t Task_PROCESSINGHandle;
const osThreadAttr_t Task_PROCESSING_attributes = {
  .name = "Task_PROCESSING",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
/* Definitions for myCountingSem01 */
osSemaphoreId_t myCountingSem01Handle;
const osSemaphoreAttr_t myCountingSem01_attributes = {
  .name = "myCountingSem01"
};
/* USER CODE BEGIN PV */
volatile page_t page = PAG_CONFIG;       // 0: Config, 1: Diag, 2: Run
//volatile uint8_t subpage = 0;    // 0: Resistencia, 1: Capacitor
//volatile uint8_t DP = 0;         // Páginas de Diagnóstico (0 a 4)

// Traemos los handlers que generó CubeMX para FreeRTOS (los nombres pueden variar según tu config)
extern osMessageQueueId_t screenQueueHandle;
//extern osSemaphoreId_t    medicionSemHandle;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM2_Init(void);
void StartTask_DISPLAY(void *argument);
void StartTask_GUI(void *argument);
void StartTask_DIAGNOSTIC(void *argument);
void StartTask_PROCESSING(void *argument);

/* USER CODE BEGIN PFP */
FSMState fsm_process_event(FSMState current, FSMEvent event);
FSMEvent Leer_Hardware_Encoder(void);
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
  MX_ADC1_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
//  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  SSD1306_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of myCountingSem01 */
  myCountingSem01Handle = osSemaphoreNew(2, 0, &myCountingSem01_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (16, sizeof(uint16_t), &myQueue01_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of task_DISPLAY */
  task_DISPLAYHandle = osThreadNew(StartTask_DISPLAY, NULL, &task_DISPLAY_attributes);

  /* creation of Task_GUI */
  Task_GUIHandle = osThreadNew(StartTask_GUI, NULL, &Task_GUI_attributes);

  /* creation of Task_DIAGNOSTIC */
  Task_DIAGNOSTICHandle = osThreadNew(StartTask_DIAGNOSTIC, NULL, &Task_DIAGNOSTIC_attributes);

  /* creation of Task_PROCESSING */
  Task_PROCESSINGHandle = osThreadNew(StartTask_PROCESSING, NULL, &Task_PROCESSING_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

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
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 10;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 10;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Rx_Pin Ri_Pin */
  GPIO_InitStruct.Pin = Rx_Pin|Ri_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : Cref_Pin Cx_Pin RiA5_Pin */
  GPIO_InitStruct.Pin = Cref_Pin|Cx_Pin|RiA5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Encoder_PUSH_Pin */
  GPIO_InitStruct.Pin = Encoder_PUSH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Encoder_PUSH_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* USER CODE BEGIN 4 */
FSMState fsm_process_event(FSMState current, FSMEvent event) {
    FSMState next = current;
    switch (current) {
        case STATE_IDLE:
            if (event == EVENT_PUSH) {
                page = PAG_CONFIG;
                next = STATE_MENU;
            }
            break;

        case STATE_MENU:
            if (event == EVENT_H)  page = (page + 1) % 3;
            if (event == EVENT_AH) page = (page + 2) % 3; // Evita índices negativos
            if (event == EVENT_PUSH) {
                if (page == PAG_CONFIG)      { page = PAG_RES; next = STATE_SUBMENU; }
                else if (page == PAG_DIAG) { page = PAG_T1; next = STATE_DIAGNOSTIC; }
                else if (page == PAG_RUN) { next = STATE_RUN; }
            }
            if (event == EVENT_ERROR) next = STATE_ERROR;
            break;

        case STATE_SUBMENU:
//            if (event == EVENT_H){
////            	if (page == PAG_RES) page = PAG_CAP;
////            	if (page == pag_cap) page = PAG_RES;
//            }
            if (event == EVENT_H)  page = PAG_RES + (page-(PAG_RES-1) ) % 2;
            if (event == EVENT_AH) page = PAG_RES + (page-(PAG_RES-1) ) % 2; // Alterna entre 0 y 1
            if (event == EVENT_PUSH) {
                // Acá podés guardar tu variable global: ej. tipo_medicion = subpage;
                next = STATE_MENU;
                page = PAG_CONFIG; //SE RESETEA LA PAGINA PORQUE SE SALE DEL SUBMENÚ AL MENÚ.
            }
            if (event == EVENT_ERROR) next = STATE_ERROR;
            break;

        case STATE_RUN:
            if (event == EVENT_PUSH)  next = STATE_MENU;
            if (event == EVENT_ERROR) next = STATE_ERROR;
            break;

        case STATE_DIAGNOSTIC:
            if (event == EVENT_H) page = PAG_T1 + (page - (PAG_T1 - 1)) % 6;
            if (event == EVENT_AH) page = PAG_FACU - ((PAG_FACU + 1) - page) % 6;
            if (event == EVENT_ERROR) next = STATE_ERROR;
            if (event == EVENT_PUSH){
            		next = STATE_MENU;
            		page = PAG_CONFIG;
            }
            break;

        case STATE_ERROR:
            if (event == EVENT_PUSH) {
                // Al presionar el encoder en pantalla de error, reiniciamos a IDLE
                page = 0;
                next = STATE_IDLE;
            }
            break;
        default: break;
    }
    return next;
}

FSMEvent Leer_Hardware_Encoder(void) {
    static int32_t cnt_anterior = 0;
    // Leemos el contador del Timer (reemplazá htim2 por el tuyo)
    int32_t cnt_actual = (int32_t)__HAL_TIM_GET_COUNTER(&htim2);
    FSMEvent evento = EVENT_NINGUNO;

    // 1. Detección de Giro (gracias a los Pull-Ups ya no oscilará solo)
    if (cnt_actual != cnt_anterior) {
		if (cnt_actual > (cnt_anterior + 3)) {
			evento = EVENT_H;
			 cnt_anterior = cnt_actual;
		}

		if (cnt_actual < (cnt_anterior - 3)) {
			evento = EVENT_AH;
		 cnt_anterior = cnt_actual;
		}
    }

	if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET){
//		HAL_Delay(300);
		osDelay(300);

		evento = EVENT_PUSH;
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//		osDelay(50);
//		HAL_Delay(30);
	}

//    // 2. Detección del botón PUSH (Pin con Pull-Up activo, entrada en BAJO al presionar)
//    static uint8_t boton_presionado = 0;
//    if (HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_12) == GPIO_PIN_RESET) { // Reemplazá por tu pin
//        if (!boton_presionado) {
//            boton_presionado = 1;
//            evento = EVENT_PUSH;
//        }
//    } else {
//        boton_presionado = 0;
//    }
    return evento;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartTask_DISPLAY */
/**
  * @brief  Function implementing the task_DISPLAY thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask_DISPLAY */
void StartTask_DISPLAY(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask_GUI */
/**
* @brief Function implementing the Task_GUI thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_GUI */
void StartTask_GUI(void *argument)
{
  /* USER CODE BEGIN StartTask_GUI */
	FSMState estado_actual = STATE_IDLE;
	FSMState estado_anterior = STATE_ERROR; // Forzamos disparar el primer envío
	FSMEvent evento = EVENT_NINGUNO;
	screenMsg_t msg_pantalla;

	// Inicializamos el modo Encoder del Timer por Hardware
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  /* Infinite loop */
  for(;;)
  {
		// 1. Leer el encoder y botón
		evento = Leer_Hardware_Encoder();
//		// 2. Consultar si Diagnóstico inyectó una alerta de forma asrincrónica
//		extern volatile uint8_t Alerta_Diagnostico_Activa; // Bandera global de la otra tarea
//		if (Alerta_Diagnostico_Activa == 1) {
//			evento = EVENT_ERROR;
//			Alerta_Diagnostico_Activa = 0;
//		}
		osDelay(30);

		// 3. Si pasó algo, procesamos y notificamos
		if (evento != EVENT_NINGUNO) {
			estado_anterior = estado_actual;
			estado_actual = fsm_process_event(estado_actual, evento);

			// Sincronización: Si entramos a MODO RUN, soltamos el semáforo para el multímetro
			if (estado_actual == STATE_RUN && estado_anterior != STATE_RUN) {
				// 3. Si pasó algo, procesamos y notificamos
				    if (evento != EVENT_NINGUNO) {
				        estado_anterior = estado_actual;
				        estado_actual = fsm_process_event(estado_actual, evento);

				        // Sincronización: Si entramos a MODO RUN, soltamos el semáforo para el multímetro
				        if (estado_actual == STATE_RUN && estado_anterior != STATE_RUN) {
				            osSemaphoreRelease(myCountingSem01Handle);
				        }

				        // Preparamos el paquete de datos para la pantalla
				        msg_pantalla.estado = estado_actual;
				        msg_pantalla.pagina = page;

				        // Se lo mandamos a la cola de la pantalla (espera 0, no bloquea la GUI)
				        osMessageQueuePut(task_DISPLAYHandle, &msg_pantalla, 0, 0);
				    }

				    // 30ms de Delay: Filtra rebotes y asegura respuestas menores a 20ms
				    osDelay(30);
			}

			// Preparamos el paquete de datos para la pantalla
			msg_pantalla.estado = estado_actual;
			msg_pantalla.pagina = page;



			// Se lo mandamos a la cola de la pantalla (espera 0, no bloquea la GUI)
			osMessageQueuePut(myQueue01Handle, &msg_pantalla, 0, 0);
		}

		// 30ms de Delay: Filtra rebotes y asegura respuestas menores a 20ms
//		OSDELAY(30);

  }
  /* USER CODE END StartTask_GUI */
}

/* USER CODE BEGIN Header_StartTask_DIAGNOSTIC */
/**
* @brief Function implementing the Task_DIAGNOSTIC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_DIAGNOSTIC */
void StartTask_DIAGNOSTIC(void *argument)
{
  /* USER CODE BEGIN StartTask_DIAGNOSTIC */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask_DIAGNOSTIC */
}

/* USER CODE BEGIN Header_StartTask_PROCESSING */
/**
* @brief Function implementing the Task_PROCESSING thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_PROCESSING */
void StartTask_PROCESSING(void *argument)
{
  /* USER CODE BEGIN StartTask_PROCESSING */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartTask_PROCESSING */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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

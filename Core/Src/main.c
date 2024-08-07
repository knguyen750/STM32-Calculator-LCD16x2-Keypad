/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "memorymap.h"
#include "usart.h"
#include "usb.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <LCD16x2.h>
#include <DWT_Delay.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
GPIO_InitTypeDef myGPIO_InitStruct = {0};
uint32_t previousTimeInms = 0;
uint32_t currentTimeInms = 0;
uint8_t key_ascii = 0;
char    *key_char;
int     result;
char msg[32];
char result_disp[10];
char *operand_buf;
char *current_operator;
int result              = 0;
int operand;
int cursor_pos          = 1;
char* prev_key_char;
int char_count_bot      = 0;
int char_count_top      = 0;
int disp_pos            = 0;
int first_char_operand_seen  = 0;
int first_operand_seen  = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
void displayResult(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Sums the most recent operator and operand inputs with the current result value
// Allows continuous inputs from 
void displayResult(void) {
  int operand = atoi(operand_buf); // convert from string to integer
  // 1) checks if last operator keypress is + or -
  // 2) perform integer calculation
  if (strcmp(current_operator, "+") == 0) {
    result = result + operand;
  }
  else if (strcmp(current_operator, "-") == 0) {
    result = result - operand;
  }
  LCD_Set_Cursor(2, 1); // set cursor to second row which contains the result of inputs
  sprintf(result_disp, "%d", result); // convert integer result to char* type
  LCD_Write_String(result_disp); // send operand character to LCD
  char_count_bot = strlen(result_disp); // track the number of characters displayed on second row
  memset(operand_buf,0,strlen(operand_buf)); // reset operand buffer to all zeroes
  // reset operand and flag values 
  operand = 0;
  first_char_operand_seen = 0;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
 
  current_operator        = malloc(strlen(key_char)+1);
  operand_buf             = malloc(strlen(key_char)+1);
  
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_PCD_Init();
  MX_MEMORYMAP_Init();
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, GPIO_PIN_SET);

  HAL_UART_Transmit(&huart1, (uint8_t*)"\033[0;0H", strlen("\033[0;0H"), HAL_MAX_DELAY);
  HAL_UART_Transmit(&huart1, (uint8_t*)"\033[2J", strlen("\033[2J"), HAL_MAX_DELAY);

  LCD_Init();
  LCD_Clear();
  LCD_Set_Cursor(1, 1);
  //LCD_Write_String("HI");
  //LCD_Set_Cursor(2, 1);
  //LCD_Write_String("FAREWELL");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    
    if (key_ascii)
    {
      
      // "A" or "B" keypress: + and - operators, respectively
      if (((strcmp(key_char, "+")) == 0) || ((strcmp(key_char, "-")) == 0)) {
        if (first_operand_seen == 0) { // processes any additional operands/operator inputs after first two operands
          result = atoi(operand_buf);
        }
        else {
          displayResult(); // calc and display result of PREVIOUS operator/operand and CURRENT result
        }
        strcpy(current_operator, key_char); // get CURRENT operator keypress 
        LCD_Set_Cursor(1, cursor_pos);
        LCD_Write_String(current_operator);
        cursor_pos          += 1;
        char_count_top      += 1; // increment total character count of top row
        first_operand_seen   = 1; // when first operator seen, we know the char buffer for the first operand is constructed 
        first_char_operand_seen   = 0; // flag to know that next operand is new (for strcpy vs. strcat)
      }
      // "C" keypress: Clear LCD
      else if ((strcmp(key_char, "C")) == 0) { 
        
        LCD_Home(); // Returns display to original position.
        LCD_Clear(); // Clear LCD values
        // Reset all counters/flags/buffers to starting position
        char_count_top = 0;
        char_count_bot = 0;
        disp_pos = 0;
        memset(operand_buf,0,strlen(operand_buf));
        memset(current_operator,0,strlen(current_operator));
        cursor_pos = 1; 
        first_char_operand_seen = 0;
        first_operand_seen = 0;
        result = 0;
      }
      // "D" keypress: Equal Operator 
      else if (((strcmp(key_char, "=")) == 0)) {
       displayResult();
       LCD_Home(); // Returns display to original position.
       LCD_Set_Cursor(2, 1);
      }
      // Bonus feature: * keypress manually shifts display register to right
      else if ((strcmp(key_char, "*")) == 0) {
        LCD_SR();
        disp_pos -= 1;
      }
      // Bonus feature: * keypress manually shifts display register to left
      else if ((strcmp(key_char, "#")) == 0) {
        LCD_SL();
        disp_pos += 1;
      }
      // 0 to 9 keypress
      else {
        // Store and build a single operand's characters in a buffer until operator key is pressed
        if ((first_char_operand_seen == 0)) { 
          strcpy(operand_buf, key_char); // first char of operand will be copied into buffer instead of concatenated
          first_char_operand_seen = 1;
        }
        else if ((first_char_operand_seen == 1)) { 
          strcat(operand_buf, key_char); // subsequent chars are concatenated
        }
        // Display the keypress char
        LCD_Set_Cursor(1, cursor_pos); // Set cursor position to top row 
        cursor_pos = cursor_pos+1; // Shift cursor to right for display of current kepyress input
        LCD_Write_String(key_char); // send operand character to LCD
        char_count_top += 1; // increment total character count of top row
        // DEBUG: print char_count top
        sprintf(msg, "\r\nchar_count_top:  %d", char_count_top);
        HAL_UART_Transmit(&huart1, (uint8_t*)msg,  strlen(msg), HAL_MAX_DELAY); 
        sprintf(msg, "\r\nchar_count_bot:  %d", char_count_bot);
        HAL_UART_Transmit(&huart1, (uint8_t*)msg,  strlen(msg), HAL_MAX_DELAY); 
        // DEBUG: print char_count top
      }
      // Automatically shifts display if characters in top or bottom row reach max. displayable value
      if ((char_count_top >= 15 || char_count_bot >= 15)) {
        LCD_Set_Auto_Shift(1); 
      }
      else {
        LCD_Set_Auto_Shift(0);
      }
      // DEBUG: Send operand ASCII character to USART 
      sprintf(msg, "\r\nKeypad Button pressed:  %c", key_ascii); 
      HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY); 
      key_ascii = 0;
      // DEBUG: Send operand ASCII character to USART 
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

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_MEDIUMHIGH);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE
                              |RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 32;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK4|RCC_CLOCKTYPE_HCLK2
                              |RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK2Divider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.AHBCLK4Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SMPS;
  PeriphClkInitStruct.SmpsClockSelection = RCC_SMPSCLKSOURCE_HSI;
  PeriphClkInitStruct.SmpsDivSelection = RCC_SMPSCLKDIV_RANGE0;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN Smps */

  /* USER CODE END Smps */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  currentTimeInms = HAL_GetTick();
  if (currentTimeInms - previousTimeInms > 200) {
    myGPIO_InitStruct.Pin = C1_Pin|C2_Pin|C3_Pin|C4_Pin;
    myGPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    myGPIO_InitStruct.Pull = GPIO_PULLDOWN;
    myGPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &myGPIO_InitStruct);

    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, 1);
    HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, 0);
    HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, 0);
    HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, 0);

    if(GPIO_Pin == C1_Pin && HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))
    {
      key_ascii = 49; //ASCII value of 1
      key_char  = "1";
    }
    else if(GPIO_Pin == C2_Pin && HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))
    {
      key_ascii = 50; //ASCII value of 2
      key_char  = "2";
    }
    else if(GPIO_Pin == C3_Pin && HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))
    {
      key_ascii = 51; //ASCII value of 3
      key_char  = "3";
    }
    else if(GPIO_Pin == C4_Pin && HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))
    {
      key_ascii = 65; //ASCII value of A
      key_char  = "+";
    }

    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, 0);
    HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, 1);
    HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, 0);
    HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, 0);

    if(GPIO_Pin == C1_Pin && HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))
    {
      key_ascii = 52; //ASCII value of 4
      key_char  = "4";
    }
    else if(GPIO_Pin == C2_Pin && HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))
    {
      key_ascii = 53; //ASCII value of 5
      key_char  = "5";
    }
    else if(GPIO_Pin == C3_Pin && HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))
    {
      key_ascii = 54; //ASCII value of 6
      key_char  = "6";
    }
    else if(GPIO_Pin == C4_Pin && HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))
    {
      key_ascii = 66; //ASCII value of B
      key_char  = "-";
    }

    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, 0);
    HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, 0);
    HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, 1);
    HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, 0);

    if(GPIO_Pin == C1_Pin && HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))
    {
      key_ascii = 55; //ASCII value of 7
      key_char  = "7";
    }
    else if(GPIO_Pin == C2_Pin && HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))
    {
      key_ascii = 56; //ASCII value of 8
      key_char  = "8";
    }
    else if(GPIO_Pin == C3_Pin && HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))
    {
      key_ascii = 57; //ASCII value of 9
      key_char  = "9";
    }
    else if(GPIO_Pin == C4_Pin && HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))
    {
      key_ascii = 67; //ASCII value of C
      key_char  = "C";
    }

    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, 0);
    HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, 0);
    HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, 0);
    HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, 1);

    if(GPIO_Pin == C1_Pin && HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin))
    {
      key_ascii = 42; //ASCII value of *
      key_char  = "*";
    }
    else if(GPIO_Pin == C2_Pin && HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin))
    {
      key_ascii = 48; //ASCII value of 0
      key_char  = "0";
    }
    else if(GPIO_Pin == C3_Pin && HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin))
    {
      key_ascii = 35; //ASCII value of #
      key_char  = "#";
    }
    else if(GPIO_Pin == C4_Pin && HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin))
    {
      key_ascii = 68; //ASCII value of D
      key_char  = "=";
    }

    HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, 1);
    HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, 1);
    HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, 1);
    HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, 1);

    /*Configure GPIO pins : PB6 PB7 PB8 PB9 back to EXTI*/
    myGPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    myGPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(C1_GPIO_Port, &myGPIO_InitStruct);

    previousTimeInms = currentTimeInms;
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

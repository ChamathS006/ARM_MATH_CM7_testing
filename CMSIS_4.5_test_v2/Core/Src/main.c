/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "string.h"
#include "arm_math.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WINDOW_SIZE 4 	// Window size of array to calculate covariance
#define NUM_OF_VARIABLES 3 	// Number of variables from sensors for calculating covariance
#define TOTAL_SIZE 12 // WINDOW_SIZE * NUM_OF_VARIABLES

#define SQUARE_SIZE 9 // NUM_OF_VARIABLES^2
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_HS_USB_Init(void);
/* USER CODE BEGIN PFP */
//arm_status mat_f32_check_equal(arm_matrix_instance_f32 matrixA, arm_matrix_instance_f32 matrixB);
//
//arm_status mat_mult_f32_test(arm_matrix_instance_f32 matrixA, arm_matrix_instance_f32 matrixB, arm_matrix_instance_f32 matrixTrue);
//
//arm_status mat_trans_f32_test(arm_matrix_instance_f32 matrixA, arm_matrix_instance_f32 matrixTrue);
//
//void vec_f32_demean(float32_t dataVector[WINDOW_SIZE], float32_t demeanedVector[WINDOW_SIZE]);
//
//arm_status create_data_matrix(arm_matrix_instance_f32* uninitialised_matrix, int numOfArrays, ...);
//
//arm_status create_data_matrix_f32_test_3_arrays(float32_t array1[9], float32_t array2[9], float32_t array3[9], float32_t arrayTrue[27]);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

arm_status mat_f32_check_equal(arm_matrix_instance_f32 matrixA, arm_matrix_instance_f32 matrixB) {
	/*
	 * Returns ARM_MATH_SIZE_MISMATCH if sizes aren't equal
	 * Returns ARM_MATH_SUCCESS if matrices are equal
	 * Returns ARM_MATH_TEST_FAILURE if matrices aren't equal
	 *
	 * */

	// Check if the rows and cols match up in number
	int test_nRows = (matrixA.numRows == matrixB.numRows);
	int test_nCols = (matrixA.numCols == matrixB.numCols);

	// If there's a mismatch, then return ARM_MATH_SIZE_MISMATCH immediately
	if (test_nRows * test_nCols == 0) {
		return ARM_MATH_SIZE_MISMATCH;
	}

	// Since rows and cols match, we need to check every entry
	for (int i = 0; i < (matrixA.numRows) * (matrixA.numCols); i++) {
		if (matrixA.pData[i] != matrixB.pData[i]) {
			return ARM_MATH_TEST_FAILURE;
		}
	}

	// If it's fine, then return ARM_MATH_SUCCESS
	return ARM_MATH_SUCCESS;
}

arm_status mat_mult_f32_test(arm_matrix_instance_f32 matrixA, arm_matrix_instance_f32 matrixB, arm_matrix_instance_f32 matrixTrue) {
	/*
	 * Checks if matrixA * matrixB equals matrixTrue
	 * Returns ARM_MATH_SUCCESS if the test is passed
	 * Returns ARM_MATH_TEST_FAILURE if the test fails
	 * Returns ARM_MATH_SIZE_MISMATCH if the sizes of each matrix are incompatible
	 * */
	// Initialise variables
	arm_matrix_instance_f32 matrixC; //C. Meant to be C = AB

	// Initialise matrixC to store the result of matrixA * matrixB
	uint16_t matrixC_size = matrixA.numRows * matrixB.numCols;
	float32_t pDataC[matrixC_size];
	arm_mat_init_f32(&matrixC, matrixA.numRows, matrixB.numCols, pDataC);

	// Multiply A and B and store in C. Status stores error info if necessary
	arm_status status = arm_mat_mult_f32(&matrixA, &matrixB, &matrixC);

	// If multiplication failed, then return it
	// If not, then check if the result is equal to the true result and return status

	if (status == ARM_MATH_SUCCESS) {
		status = mat_f32_check_equal(matrixC, matrixTrue);
	}
	free(pDataC);
	return status;
}

arm_status mat_trans_f32_test(arm_matrix_instance_f32 matrixA, arm_matrix_instance_f32 matrixTrue) {
	// Initialise variables
	arm_matrix_instance_f32 matrixAT;
	uint16_t sizeMatrixAT = matrixA.numRows * matrixA.numCols;
	float32_t pDataAT[sizeMatrixAT];
	arm_mat_init_f32(&matrixAT, matrixA.numCols, matrixA.numRows, pDataAT);

	// Transpose A and store in AT
	arm_status status = arm_mat_trans_f32(&matrixA, &matrixAT);

	if (status == ARM_MATH_SIZE_MISMATCH) {
		Error_Handler();
	}

	// Check if the result matches the true answer
	status = mat_f32_check_equal(matrixAT, matrixTrue);

	return status;
}

void vec_f32_demean(float32_t dataVector[WINDOW_SIZE]/*, float32_t demeanedVector[WINDOW_SIZE]*/) {
/*
 *
 * */
	// Initialise variables
	float32_t meanValue = 0; // Stores mean of the dataVector input

	// first, get the mean of this 1 x WINDOW_SIZE vector and store in meanValue
	arm_mean_f32(dataVector, (uint32_t) WINDOW_SIZE, &meanValue);

	// Now offset the input vector by the mean value
	arm_offset_f32(dataVector, -meanValue, dataVector, (uint32_t) WINDOW_SIZE);

}

arm_status create_data_matrix_f32(arm_matrix_instance_f32* uninitialised_matrix, int numOfArrays, ...) {
	/* This function takes in an uninitialised matrix ptr, a number of arrays, and those float32_t arrays themselves
	 * It checks that there is an appropriate number of inputs and that all inputs are of the appropriate size
	 * If this is true, then the matrix is initialised formally with each array being a row of the matrix.
	 *
	 *
	 * uninitialised_matrix: Uninitialised matrix instance. This will be altered to turn into the relevant matrix
	 * numOfArrays: A counter to check how many arrays were passed in
	 * ...: Variable number of arrays passed in. Arrays of float32_t type to match the va_arg calls in this function
	 * 	NOTE: Each array should have a length of WINDOW_SIZE, but as of 18/07/2022
	 *
	 * Returns ARM_MATH_SUCCESS if a matrix was initialised correctly
	 * If numOfArrays doesn't match NUM_OF_VARIABLES, returns with ARM_MATH_SIZE_MISMATCH
	 * Also sets uninitialised_matrix to have the relevant size (nRows = numOfArrays ; nCols = WINDOW_SIZE)
	 * 	Each row corresponds to a different variable. They represent the 0th to (WINDOW_SIZE - 1)th sample
	 * 		(eg: XAccel data from 1st sample to 100th sample, corresponding )
	 * 	Each column represents the nth data entry of all variables
	 * 		(eg: n = 4 matches column 3: Column 3 has the 4th data entry for XAccel, YAccel, ZAccel, Altitude, Pressure, Temp)
	 *
	 * Eg: create_data_matrix_f32(dummy_matrix, 3, XAccelVector[WINDOW_SIZE], YAccelVector[WINDOW_SIZE], ZAccelVector[WINDOW_SIZE]);
	 * */
	// if numOfArrays != NUM_OF_VARIABLES, then return size mismatch immediately
	if (numOfArrays != NUM_OF_VARIABLES) {
		return ARM_MATH_SIZE_MISMATCH;
	}
	// Declare variable argument variables and other variables
	va_list valist;
//	int outputVector_size = WINDOW_SIZE * numOfArrays;
	float32_t outputVector[TOTAL_SIZE];

	// Start accessing the arguments
	va_start(valist, numOfArrays);

	// Since all arguments from here should be float32_t arrays, declare a dummy array to store it
	float32_t* argument = NULL;

	for (int i = 0; i < numOfArrays; i++) {
		// Get next argument
		argument = va_arg(valist, float32_t*);

		// If the length of this argument doesn't match WINDOW_SIZE, then return size mismatch
//		if ((sizeof(argument))/(sizeof(float32_t)) != WINDOW_SIZE) {
//			return ARM_MATH_SIZE_MISMATCH;
//		}

		// Add it to the output array
		for (int j = 0; j < WINDOW_SIZE; j++) {
			outputVector[i * WINDOW_SIZE + j] = argument[j];
		}
	}
	// Stop accessing arguments
	va_end(valist);

	// Output vector now has everything we need
	// Properly initialise the matrix so that it has everything it needs
	arm_mat_init_f32(uninitialised_matrix, numOfArrays, (uint16_t) WINDOW_SIZE, outputVector);

	// Seems like the function works; return ARM_MATH_SUCCESS
	return ARM_MATH_SUCCESS;
}

arm_status create_covariance_matrix_f32(arm_matrix_instance_f32* dataMatrix, int N, arm_matrix_instance_f32* covarianceMatrix) {
	/*
	* Inputs:
	* dataMatrix: A matrix with all the data entries in them, and demeaned already
	* 	The format should be:
	* 		Rows representing different variables
	* 		Columns representing different samples
	* 	Note that the create_data_matrix_f32 function turns the uninitialised matrix input into a
	* 	matrix with the compatible format for this function
	*
	* N: the number of samples (should be WINDOW_SIZE anyway). Bessel's correction is done internally anyway
	* covarianceMatrix: An uninitialised matrix with known dimensions
	* 	(NUM_OF_VARIABLES x NUM_OF_VARIABLES)
	*
	* Outputs:
	* status to show if the function completed the steps.
	* 	Furthermore, the covarianceMatrix input will now be initialised with the correct data
	* */

	// Initialise status variable
	arm_status status;

	// Initialise a new matrix to store the transpose of dataMatrix
	arm_matrix_instance_f32* dataMatrixT = dataMatrix - ((TOTAL_SIZE+1) * sizeof(arm_matrix_instance_f32));
//	uint16_t dataMatrixT_size = (dataMatrix.numRows) * (dataMatrix.numCols);
//	float32_t pData_dataMatrixT[TOTAL_SIZE];
	float32_t* pData_dataMatrixT = dataMatrix->pData - ((TOTAL_SIZE+1) * sizeof(float32_t));
	arm_mat_init_f32(dataMatrixT, dataMatrix->numCols, dataMatrix->numRows, pData_dataMatrixT);

	// Initialise a new matrix to store the result of dataMatrix * dataMatrixT
	arm_matrix_instance_f32* tempMatrix = (dataMatrixT) - ((TOTAL_SIZE+1) * sizeof(arm_matrix_instance_f32));
//	uint16_t tempMatrix_size = (dataMatrix.numRows) * (dataMatrix.numRows); // Due to the order of multiplication
//	float32_t pData_tempMatrix[SQUARE_SIZE];
	float32_t* pData_tempMatrix = (dataMatrixT->pData) - ((TOTAL_SIZE+1) * sizeof(float32_t));
	arm_mat_init_f32(tempMatrix, (dataMatrix->numRows), (dataMatrix->numRows), pData_tempMatrix);

	// Properly initialise the covariance matrix
//	uint16_t covarianceMatrix_size = (dataMatrix.numRows) * (dataMatrix.numRows);
//	float32_t pData_covarianceMatrix[SQUARE_SIZE];
	float32_t* pData_covarianceMatrix = (tempMatrix->pData) - ((TOTAL_SIZE+1) * sizeof(float32_t));
	arm_mat_init_f32(covarianceMatrix, dataMatrix->numRows, dataMatrix->numRows, pData_covarianceMatrix);

	// Transpose dataMatrix and store in dataMatrixT
	status = arm_mat_trans_f32(dataMatrix, &dataMatrixT);

	if (status != ARM_MATH_SUCCESS) {
		return status;
	}

	// Now do dataMatrix * dataMatrixT and store into tempMatrix
	status = arm_mat_mult_f32(dataMatrix, &dataMatrixT, &tempMatrix);

	if (status != ARM_MATH_SUCCESS) {
		return status;
	}

	// Finally, scale each result by 1/(N-1) = 1/((uint16_t) WINDOW_SIZE - 1) (Bessel's correction
	float32_t scaleFactor = ((float32_t) 1)/(N-1);
	status = arm_mat_scale_f32(&tempMatrix, scaleFactor, covarianceMatrix);
	if (status != ARM_MATH_SUCCESS) {
		return status;
	}

	return status;
}

arm_status create_data_matrix_f32_test_3_arrays(float32_t array1[9], float32_t array2[9], float32_t array3[9], float32_t arrayTrue[27]) {
	// Initialise variables
	arm_matrix_instance_f32 test_matrix;
	arm_matrix_instance_f32 true_matrix;
	uint16_t arrayRows = 1;
	uint16_t arrayCols = 3;

	uint16_t arrayTrueRows = 3;
	uint16_t arrayTrueCols = 9;

	arm_status status;

	// Set up the true matrix
	arm_mat_init_f32(&true_matrix, arrayTrueRows, arrayTrueCols, arrayTrue);

	// Call the function
	status = create_data_matrix_f32(&test_matrix, 3, array1, array2, array3);

	// Check if the test matrix is the same as the true matrix
	status = mat_f32_check_equal(test_matrix, true_matrix);

	return status;

}

arm_status create_covariance_matrix_f32_test_3_arrays(float32_t array1[WINDOW_SIZE], float32_t array2[WINDOW_SIZE], float32_t array3[WINDOW_SIZE], float32_t arrayTrue[9]) {
	/*
	 * Test structure:
	 * 	Demean array1, array2, and array3 respectively using the vec_f32_demean function
	 * 	Use the resulting vectors to create a data matrix using the create_data_matrix_f32 function
	 * 	From there, pass the resulting data_matrix into the create_covariance_matrix_f32 function
	 *
	 * 	Check if the final result matches the expected result generated on MATLAB (format long just in case)
	 *	Return status if the status is not ARM_MATH_SUCCESS at some point
	 *	Return ARM_MATH_SUCCESS if the test is passed
	 *	Return ARM_MATH_TEST_FAILURE if the test fails
	 *
	 * */

	// Initialise general variables
	uint16_t numOfRows = 3;
	uint16_t numOfCols = 4;
	arm_status status;

	// Initialise variables for demeaning step
//	float32_t demeanedArray1[WINDOW_SIZE];
//	float32_t demeanedArray2[WINDOW_SIZE];
//	float32_t demeanedArray3[WINDOW_SIZE];

	// Create the true covariance matrix (size of arrayTrue should be numOfRows^2)
	arm_matrix_instance_f32 covarianceMatrixTrue;
	arm_mat_init_f32(&covarianceMatrixTrue, numOfRows, numOfRows, arrayTrue);

	// Initialise variables for data_matrix creation step
	arm_matrix_instance_f32 dataMatrix;

	int size_arm_mat_instance_f32 = sizeof(dataMatrix);

	// Initialise variables for covariance creation step
	arm_matrix_instance_f32 covarianceMatrix;

	// Demean the data and store in appropriate vectors
//	vec_f32_demean(array1, demeanedArray1);
//	vec_f32_demean(array2, demeanedArray2);
//	vec_f32_demean(array3, demeanedArray3);

	vec_f32_demean(array1);
	vec_f32_demean(array2);
	vec_f32_demean(array3);

	// Use demeanedArrays to make a data_matrix
//	status = create_data_matrix_f32(&dataMatrix, numOfRows, demeanedArray1, demeanedArray2, demeanedArray3);

	status = create_data_matrix_f32(&dataMatrix, numOfRows, array1, array2, array3);

	size_arm_mat_instance_f32 = sizeof(dataMatrix);

	if (status != ARM_MATH_SUCCESS) {
		return status;
	}



	// Use dataMatrix to make covariance matrix
	status = create_covariance_matrix_f32(&dataMatrix, numOfCols, &covarianceMatrix);

	if (status != ARM_MATH_SUCCESS) {
		return status;
	}



	// Check that this covarianceMatrix matches the true matrix
	status = mat_f32_check_equal(covarianceMatrix, covarianceMatrixTrue);

	return status;
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
  MX_USART3_UART_Init();
  MX_USB_OTG_HS_USB_Init();
  /* USER CODE BEGIN 2 */

  // Initialise some variables
  arm_matrix_instance_f32 matrixA; //A
  arm_matrix_instance_f32 matrixB; //B

  arm_matrix_instance_f32 matrixTrue_mat_mult_f32; // True result to compare against
  arm_matrix_instance_f32 matrixTrue_mat_trans_f32;

  uint16_t nRowsA = 3;
  uint16_t nColsA = 3;
  float32_t pDataA[] = {1,2,3,4,5,6,7,8,9};

  uint16_t nRowsB = 3;
  uint16_t nColsB = 3;
  float32_t pDataB[] = {8,1,3,5,2,4,8,1,5};

  float32_t pDataC[] = {9,8,7,6,5,4,3,2,1};
  float32_t pDataTrue_create_data_matrix_f32_test_3_arrays[] = {1,2,3,4,5,6,7,8,9,8,1,3,5,2,4,8,1,5,9,8,7,6,5,4,3,2,1};

  float32_t pDataX[] = {1,2,3,4};
  float32_t pDataY[] = {4,9,6,1};
  float32_t pDataZ[] = {9,2,4,6};

  uint16_t nRowsTrue_mat_mult_f32 = nRowsA;
  uint16_t nColsTrue_mat_mult_f32 = nColsB;
  float32_t pDataTrue_mat_mult_f32[] = {18,5,11,44,11,25,70,17,39};
//  float32_t pDataTrue_covariance_f32[] = {
//		  19,8,	6,4,-3,-5,-5,-16,-16,8,14.3333333333333,8.66666666666667,3,4,-1.66666666666667,-10,-3.66666666666667,-12,6,8.66666666666667,5.33333333333333,2,2,-1.33333333333333,-6,-3.33333333333333,-8,4,3,2,1,0,-1,-2,-3,-4,-3,4,2,0,3,1,-3,4.00000000000000,0,-5,-1.66666666666667,-1.33333333333333,-1,1,1.33333333333333,1,4.33333333333333,4,-5,-10,-6,-2,-3,1,7,2.00000000000000,8,-16,-3.66666666666667,-3.33333333333333,-3,4.00000000000000,4.33333333333333,2.00000000000000,14.3333333333333,12,-16,-12,-8,-4,0,4,8,12,16
//  };
  float32_t pDataTrue_covariance_f32[] = {16.3333,-2.3333,1.1667,5.333,-2.3333,16.3333,5.8333,-9.3333,1.1667,5.8333,2.3333,-2.8333,5.3333,-9.3333,-2.8333,6.3333};

  uint16_t nRowsTrue_mat_trans_f32 = 2;
  uint16_t nColsTrue_mat_trans_f32 = 2;
  float32_t pDataTrue_mat_trans_f32[] = {8,5,1,2,3,4};

  arm_mat_init_f32(&matrixA, nRowsA, nColsA, pDataA);
  arm_mat_init_f32(&matrixB, nRowsB, nColsB, pDataB);
  arm_mat_init_f32(&matrixTrue_mat_mult_f32, nRowsTrue_mat_mult_f32, nColsTrue_mat_mult_f32, pDataTrue_mat_mult_f32);
  arm_mat_init_f32(&matrixTrue_mat_trans_f32, nRowsTrue_mat_trans_f32, nColsTrue_mat_trans_f32, pDataTrue_mat_trans_f32);

  // Call the relevant test
//  arm_status test_result = mat_mult_f32_test(matrixA, matrixB, matrixTrue_mat_mult_f32);
//  arm_status test_result = mat_trans_f32_test(matrixB, matrixTrue_mat_trans_f32);
//  arm_status test_result = create_data_matrix_f32_test_3_arrays(pDataA, pDataB, pDataC, pDataTrue_create_data_matrix_f32_test_3_arrays);
  arm_status test_result = create_covariance_matrix_f32_test_3_arrays(pDataX, pDataY, pDataZ, pDataTrue_covariance_f32);
  // Depending on the test result, blink LED's appropriately
  if (test_result == ARM_MATH_SIZE_MISMATCH) {
	  Error_Handler();
  }


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (test_result == ARM_MATH_SUCCESS) {
		  HAL_GPIO_TogglePin(Green_LED_GPIO_Port, Green_LED_Pin);
	  }
	  else if (test_result == ARM_MATH_TEST_FAILURE) {
		  HAL_GPIO_TogglePin(Red_LED_GPIO_Port, Red_LED_Pin);
	  }
	  HAL_Delay(1000);
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 24;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_HS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_HS_USB_Init(void)
{

  /* USER CODE BEGIN USB_OTG_HS_Init 0 */

  /* USER CODE END USB_OTG_HS_Init 0 */

  /* USER CODE BEGIN USB_OTG_HS_Init 1 */

  /* USER CODE END USB_OTG_HS_Init 1 */
  /* USER CODE BEGIN USB_OTG_HS_Init 2 */

  /* USER CODE END USB_OTG_HS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_FS_PWR_EN_GPIO_Port, USB_FS_PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Green_LED_Pin|Red_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Yellow_LED_GPIO_Port, Yellow_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_PWR_EN_Pin */
  GPIO_InitStruct.Pin = USB_FS_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_FS_PWR_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Green_LED_Pin Red_LED_Pin */
  GPIO_InitStruct.Pin = Green_LED_Pin|Red_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_OVCR_Pin */
  GPIO_InitStruct.Pin = USB_FS_OVCR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_FS_OVCR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_VBUS_Pin */
  GPIO_InitStruct.Pin = USB_FS_VBUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_FS_VBUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_ID_Pin */
  GPIO_InitStruct.Pin = USB_FS_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
  HAL_GPIO_Init(USB_FS_ID_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_FS_N_Pin USB_FS_P_Pin */
  GPIO_InitStruct.Pin = USB_FS_N_Pin|USB_FS_P_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : Yellow_LED_Pin */
  GPIO_InitStruct.Pin = Yellow_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Yellow_LED_GPIO_Port, &GPIO_InitStruct);

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
	  HAL_GPIO_WritePin(Red_LED_GPIO_Port, Red_LED_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(Green_LED_GPIO_Port, Green_LED_Pin, GPIO_PIN_SET);
	  HAL_GPIO_WritePin(Yellow_LED_GPIO_Port, Yellow_LED_Pin, GPIO_PIN_SET);
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


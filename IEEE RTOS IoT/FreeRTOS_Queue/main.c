/*
 * FreeRTOS V202112.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * configCREATE_SIMPLE_TICKLESS_DEMO setting (defined in FreeRTOSConfig.h) is
 * used to select between the two.  The simply blinky demo is implemented and
 * described in main_blinky.c.  The more comprehensive test and demo application
 * is implemented and described in main_full.c.
 *
 * The comprehensive demo uses FreeRTOS+CLI to create a simple command line
 * interface through a UART.
 *
 * The blinky demo uses FreeRTOS's tickless idle mode to reduce power
 * consumption.  See the notes on the web page below regarding the difference
 * in power saving that can be achieved between using the generic tickless
 * implementation (as used by the blinky demo) and a tickless implementation
 * that is tailored specifically to the MSP432.
 *
 * This file implements the code that is not demo specific.
 *
 * See http://www.FreeRTOS.org/TI_MSP432_Free_RTOS_Demo.html for instructions.
 *
 */

/* Standard includes. */
#include <stdio.h>
#include <stdbool.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Hardware includes */
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "Queue_main.h"

/*-----------------------------------------------------------*/

/* NOTE: If an IAR build results in an undefined "reference to __write" linker
error then set the printf formatter project option to "tiny" and the scanf
formatter project option to "small". */

/*
 * Set up the hardware ready to run this demo.
 */
static void prvSetupHardware( void );

extern void Task1 (void*);
extern void Task2 (void*);
extern void Task3 (void*);

QueueHandle_t Queue1;
QueueHandle_t Queue2;

/*-----------------------------------------------------------*/

int main( void )
{
	/* See http://www.FreeRTOS.org/TI_MSP432_Free_RTOS_Demo.html for instructions. */

	/* Prepare the hardware to run this demo. */
	prvSetupHardware();

	xTaskCreate(&Task1, "Tarea 1", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(&Task2, "Tarea 2", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(&Task3, "Tarea 3", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	Queue1 = xQueueCreate(3, sizeof(struct Data_t));
	Queue2 = xQueueCreate(1, sizeof(float));


	vTaskStartScheduler();
	return 0;
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);

	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

	UARTStdioConfig(0, 115200, configCPU_CLOCK_HZ);
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	IntMasterDisable();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	IntMasterDisable();
	for( ;; );
}
/*-----------------------------------------------------------*/

void *malloc( size_t xSize )
{
	/* There should not be a heap defined, so trap any attempts to call
	malloc. */
	IntMasterDisable();
	for( ;; );
}
/*-----------------------------------------------------------*/



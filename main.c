#include "stm32f4xx.h"                  // Device header
#include "FreeRTOS.h"                   // ARM.FreeRTOS::RTOS:Core
#include "task.h"                       // ARM.FreeRTOS::RTOS:Core
#include "queue.h"                      // ARM.FreeRTOS::RTOS:Core
#include "semphr.h"                     // ARM.FreeRTOS::RTOS:Core

#define LED_DELAY							1000
#define LED_SHORT_DELAY				250
#define SOS_LED_DELAY					500
#define SOS_LED_SHORT_DELAY		83
#define n											3

static xSemaphoreHandle mutex;

void delay(const int constr)
{
	TickType_t xLastWakeTime;
	const TickType_t xPeriod = pdMS_TO_TICKS(constr);
	xLastWakeTime = xTaskGetTickCount();
	
	vTaskDelayUntil(&xLastWakeTime, xPeriod); 
};

void InitHardware(void)
{
	// Enable clock for GPIO port D
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;
	
	// PD15 output mode
	GPIOD->MODER |= (1 << 30);
};

void vTaskFirst(void *pvParameters)
{
	while(1) {
		while(xSemaphoreTake(mutex, 64000000) == pdFAIL);
		delay(LED_DELAY);
		GPIOD->ODR |= (1 << 15);
		delay(LED_DELAY);
		GPIOD->ODR &= ~((uint32_t)(1 << 15));
		delay(LED_DELAY);
		GPIOD->ODR |= (1 << 15);
		delay(LED_DELAY);
		GPIOD->ODR &= ~((uint32_t)(1 << 15));
		xSemaphoreGive(mutex);
		delay(LED_DELAY);
	};
};

void vTaskSecond(void *pvParameters)
{
	uint16_t i = 0;
	while(1) {
		while(xSemaphoreTake(mutex, 64000000) == pdFAIL);
		for(i = 0; i < n; i++) {
			delay(SOS_LED_SHORT_DELAY);
			GPIOD->ODR |= (1 << 15);
			delay(SOS_LED_SHORT_DELAY);
			GPIOD->ODR &= ~((uint32_t)(1 << 15));
		};
		
		for(i = 0; i < n; i++) {
			delay(SOS_LED_DELAY);
			GPIOD->ODR |= (1 << 15);
			delay(SOS_LED_DELAY);
			GPIOD->ODR &= ~((uint32_t)(1 << 15));
		};
		
		for(i = 0; i < n; i++) {
			delay(SOS_LED_SHORT_DELAY);
			GPIOD->ODR |= (1 << 15);
			delay(SOS_LED_SHORT_DELAY);
			GPIOD->ODR &= ~((uint32_t)(1 << 15));
		};
		xSemaphoreGive(mutex);
		delay(LED_DELAY);
	};
};

int main(void)
{
	InitHardware();
	
	mutex = xSemaphoreCreateMutex();
	
	xTaskCreate(vTaskFirst, "First",
		configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vTaskSecond, "Second",
		configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	
	vTaskStartScheduler();
	
	while(1);
};

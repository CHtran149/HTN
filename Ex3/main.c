#include "stm32f10x.h"                  // Device header
#include "stm32f10x_exti.h"             // Keil::Device:StdPeriph Drivers:EXTI
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_tim.h"              // Keil::Device:StdPeriph Drivers:TIM



void Config_GPIO(){
	GPIO_InitTypeDef led;
	GPIO_InitTypeDef button;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	led.GPIO_Mode		= GPIO_Mode_Out_PP;
	led.GPIO_Pin		= GPIO_Pin_1 | GPIO_Pin_3;
	led.GPIO_Speed	= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &led);
	
	button.GPIO_Mode 		= GPIO_Mode_IPU;
	button.GPIO_Pin			= GPIO_Pin_2;
	button.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &button);
}

void Config_Nvic(){
	NVIC_InitTypeDef nvic;
	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x00);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	nvic.NVIC_IRQChannel = EXTI2_IRQn;
	nvic.NVIC_IRQChannelCmd = ENABLE;
	nvic.NVIC_IRQChannelPreemptionPriority = 0;
	nvic.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&nvic);
}
void Config_Exti(){
	EXTI_InitTypeDef exti;
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource2);
	exti.EXTI_Line		= EXTI_Line2;
	exti.EXTI_LineCmd = ENABLE;
	exti.EXTI_Mode		= EXTI_Mode_Interrupt;
	exti.EXTI_Trigger	= EXTI_Trigger_Falling;
	
	EXTI_Init(&exti);
}

void Config_Timer(){
	TIM_TimeBaseInitTypeDef timer;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	timer.TIM_ClockDivision			= 0;
	timer.TIM_CounterMode				= TIM_CounterMode_Up;
	timer.TIM_Period						= 65535;
	timer.TIM_Prescaler					= 2-1;
	timer.TIM_RepetitionCounter	=0;
	TIM_TimeBaseInit(TIM2, &timer);
}

void Delay_1ms(){
	Config_Timer();
	TIM_Cmd(TIM2, ENABLE);
	TIM_SetCounter(TIM2, 0);
	while(TIM_GetCounter(TIM2) < 18000);
	TIM_Cmd(TIM2, DISABLE);
}

void Delay_ms(unsigned int t){
	while(t--){
		Delay_1ms();
	}
}

void EXTI2_IRQHandler(){
	uint8_t led1_state = GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1);
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2) == 0){
		GPIO_WriteBit(GPIOA, GPIO_Pin_1, !led1_state);
	}
	EXTI_ClearITPendingBit(EXTI_Line2);
}


int main(){
	Config_GPIO();
	Config_Nvic();
	Config_Exti();
	GPIO_WriteBit(GPIOA, GPIO_Pin_1, 1);
	while(1){
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		Delay_ms(500);
		GPIO_SetBits(GPIOA, GPIO_Pin_3);
		Delay_ms(500);
	}
	
}
#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_dma.h"              // Keil::Device:StdPeriph Drivers:DMA
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include "string.h"
#include "stdio.h"

#define ADC_SIZE 16
uint16_t ADC_BUFFER[ADC_SIZE];

void Config_USART(){
	GPIO_InitTypeDef gpio;
	USART_InitTypeDef usart;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AF_PP;
	gpio.GPIO_Pin			= GPIO_Pin_9;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	gpio.GPIO_Mode		= GPIO_Mode_IN_FLOATING;
	gpio.GPIO_Pin			= GPIO_Pin_10;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	usart.USART_BaudRate			= 9600;
	usart.USART_HardwareFlowControl		= USART_HardwareFlowControl_None;
	usart.USART_Mode					= USART_Mode_Rx	| USART_Mode_Tx;
	usart.USART_Parity				= USART_Parity_No;
	usart.USART_StopBits			= USART_StopBits_1;
	usart.USART_WordLength		= USART_WordLength_8b;
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);
}

void usart_SendChar(char c){
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	
}

void usart_SendStr(char *str){
	while(*str != NULL){
		usart_SendChar(*str++);
	}
}

void Config_ADC(){
	GPIO_InitTypeDef gpio;
	ADC_InitTypeDef adc;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AIN;
	gpio.GPIO_Pin			= GPIO_Pin_0;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &gpio);
	
	adc.ADC_ContinuousConvMode			= ENABLE;
	adc.ADC_DataAlign								= ADC_DataAlign_Right;
	adc.ADC_ExternalTrigConv				= ADC_ExternalTrigConv_None;
	adc.ADC_Mode										= ADC_Mode_Independent;
	adc.ADC_NbrOfChannel						= 1;
	adc.ADC_ScanConvMode						= DISABLE;
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
}

void Config_DMA(){
	DMA_InitTypeDef dma;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel1);
	
	dma.DMA_PeripheralBaseAddr	= (uint32_t)&ADC1->DR;
	dma.DMA_MemoryBaseAddr			= (uint32_t)&ADC_BUFFER;
	dma.DMA_DIR									= DMA_DIR_PeripheralSRC; //truyen tu ngoai vi vao bo nho;
	dma.DMA_BufferSize					= ADC_SIZE;
	dma.DMA_PeripheralInc				= DMA_PeripheralInc_Disable;
	dma.DMA_MemoryInc						= DMA_MemoryInc_Enable;
	dma.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_HalfWord;
	dma.DMA_MemoryDataSize			= DMA_MemoryDataSize_HalfWord;
	dma.DMA_Mode								= DMA_Mode_Normal;
	dma.DMA_Priority						= DMA_Priority_High;
	dma.DMA_M2M									= DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &dma);
	DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}

void Config_NVIC() {
    NVIC_InitTypeDef nvic;
    nvic.NVIC_IRQChannel = DMA1_Channel1_IRQn;
    nvic.NVIC_IRQChannelPreemptionPriority = 0;
    nvic.NVIC_IRQChannelSubPriority = 0;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}


void DMA1_Channel1_IRQHandler(void) {
		int i;
    if (DMA_GetITStatus(DMA1_IT_TC1)) {
        DMA_ClearITPendingBit(DMA1_IT_TC1);

        usart_SendStr("DMA Transfer Complete. Data:\r\n");
        for (i = 0; i < ADC_SIZE; i++) {
            char buf[32];
            sprintf(buf, "%d\r\n", ADC_BUFFER[i]);
            usart_SendStr(buf);
        }
    }
}


int main(){
	Config_USART();
	Config_ADC();
	Config_DMA();
	while(1){
	}
	
}

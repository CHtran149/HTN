#include "stm32f10x.h"                  // Device header
#include "stm32f10x_adc.h"              // Keil::Device:StdPeriph Drivers:ADC
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include <stdio.h>
#include <string.h>

void Delay_ms(unsigned int t){
	unsigned int i, j;
	for(i=0; i<t; i++){
		for(j=0; j<0x2aff; j++);
	}
}

void Config_GPIO(){
	GPIO_InitTypeDef gpio;
	GPIO_InitTypeDef uart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_AIN;
	gpio.GPIO_Pin			= GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio);
	
	uart.GPIO_Mode		= GPIO_Mode_AF_PP;
	uart.GPIO_Pin			= GPIO_Pin_9;
	uart.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &uart);
	
	uart.GPIO_Mode		= GPIO_Mode_IN_FLOATING;
	uart.GPIO_Pin			= GPIO_Pin_10;
	uart.GPIO_Speed		= GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &uart);
}

void Config_Usart(){
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	usart.USART_BaudRate 			= 9600;
	usart.USART_HardwareFlowControl			= USART_HardwareFlowControl_None;
	usart.USART_Mode					= USART_Mode_Rx | USART_Mode_Tx;
	usart.USART_Parity				= USART_Parity_No;
	usart.USART_StopBits			= USART_StopBits_1;
	usart.USART_WordLength		= USART_WordLength_8b;
	
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);
}

void uart_SendChar(char _chr){ 
 USART_SendData(USART1, _chr); 
 while(USART_GetFlagStatus(USART1, USART_FLAG_TXE)==RESET); 
} 

struct __FILE { 
    int dummy; 
}; 
FILE __stdout; 
  
int fputc(int ch, FILE *f) { 
    uart_SendChar(ch); 
    return ch; 
} 

void Config_ADC(){
	ADC_InitTypeDef adc;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	adc.ADC_ContinuousConvMode			= ENABLE;
	adc.ADC_DataAlign								= ADC_DataAlign_Right;
	adc.ADC_ExternalTrigConv				= ADC_ExternalTrigConv_None;
	adc.ADC_Mode										= ADC_Mode_Independent;
	adc.ADC_NbrOfChannel						= 1;
	adc.ADC_ScanConvMode						= DISABLE;
	ADC_Init(ADC1, &adc);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	
}

 
unsigned int ADCx_Read(void){ 
 return(ADC_GetConversionValue(ADC1)); 
} 

int Map(int x, int in_min, int in_max, int out_min, int out_max) { 
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min; 
} 

int main(){
	uint16_t adc_value;
	int vol_mV;
	
	Config_GPIO();
	Config_Usart();
	Config_ADC();
	while(1){
		adc_value 	= ADCx_Read();
		vol_mV 			= Map(adc_value, 0, 4095, 0, 3300);
		printf("ADC: %u\t Voltage: %d mV\r\n", adc_value, vol_mV);
		Delay_ms(500);
	}
}
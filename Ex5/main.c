#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include "stm32f10x_rcc.h"              // Keil::Device:StdPeriph Drivers:RCC
#include "stm32f10x_usart.h"            // Keil::Device:StdPeriph Drivers:USART
#include <string.h>
#include <stdio.h>


void Config_GPIO(){
	GPIO_InitTypeDef gpio;
	GPIO_InitTypeDef uart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	gpio.GPIO_Mode		= GPIO_Mode_Out_PP;
	gpio.GPIO_Pin 		= GPIO_Pin_1;
	gpio.GPIO_Speed		= GPIO_Speed_50MHz;
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

void Config_Uart(){
	USART_InitTypeDef usart;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	usart.USART_BaudRate			= 9600;
	usart.USART_HardwareFlowControl		= USART_HardwareFlowControl_None;
	usart.USART_Mode					= USART_Mode_Tx | USART_Mode_Rx;
	usart.USART_Parity				= USART_Parity_No;
	usart.USART_StopBits			= USART_StopBits_1;
	usart.USART_WordLength		= USART_WordLength_8b;
	USART_Init(USART1, &usart);
	USART_Cmd(USART1, ENABLE);
}

void uart_SendChar(char c){
	USART_SendData(USART1, c);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void uart_SendStr(char *str){
	while(*str != NULL){
		uart_SendChar(*str++);
	}
}

struct __FILE { 
    int dummy; 
}; 
FILE __stdout; 
  
int fputc(int ch, FILE *f) { 
    uart_SendChar(ch); 
    return ch; 
} 

void Delay_ms(unsigned int t){
	unsigned int i, j;
	for(i = 0; i < t; i++){
		for(j = 0; j < 0x2aff; j++);
	}
}

int main(){
	Config_GPIO();
	Config_Uart();
	while(1){
		uart_SendStr("Hello from STM32!\n");
		printf("Hello CT\n");
		Delay_ms(500);
		
	}
}